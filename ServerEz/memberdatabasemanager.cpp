#include "memberdatabasemanager.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

MemberDatabaseManager::MemberDatabaseManager(const QString &dbPath, QObject *parent)
    : QObject(parent), m_dbPath(dbPath)
{
    m_connectionName = QString("MemberDB_%1").arg(reinterpret_cast<quintptr>(this));
    m_db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    m_db.setDatabaseName(m_dbPath);
}

MemberDatabaseManager::~MemberDatabaseManager()
{
    close();
}

bool MemberDatabaseManager::open()
{
    if (!m_db.open()) {
        qWarning() << "MemberDB: Cannot open database:" << m_db.lastError().text();
        return false;
    }
    return createMemberTable();
}

void MemberDatabaseManager::close()
{
    if (m_db.isOpen()) m_db.close();
    QSqlDatabase::removeDatabase(m_connectionName);
}

bool MemberDatabaseManager::createMemberTable()
{
    QSqlQuery q(m_db);
    const QString sql = R"(
      CREATE TABLE IF NOT EXISTS members (
        firstname TEXT NOT NULL,
        lastname  TEXT NOT NULL,
        email     TEXT UNIQUE NOT NULL,
        phone     TEXT,
        username  TEXT UNIQUE NOT NULL,
        password  TEXT NOT NULL
      );
    )";
    if (!q.exec(sql)) {
        qWarning() << "MemberDB: create table failed:" << q.lastError().text();
        return false;
    }
    return true;
}

bool MemberDatabaseManager::addMember(QTcpSocket *client,
                                      const QString &firstname,
                                      const QString &lastname,
                                      const QString &email,
                                      const QString &phone,
                                      const QString &username,
                                      const QString &password)
{
    if (!client) {
        qWarning() << "MemberDB: Null client in addMember";
        return false;
    }
    if (!m_db.isOpen()) {
        qWarning() << "MemberDB: addMember called but DB is not open!";
        client->write("S[ERROR][Database not open]\n");
        return false;
    }

    m_db.transaction();

    QSqlQuery chk(m_db);
    chk.prepare("SELECT COUNT(*) FROM members WHERE username = :u OR email = :e;");
    chk.bindValue(":u", username);
    chk.bindValue(":e", email);
    if (!chk.exec() || !chk.next()) {
        qWarning() << "MemberDB: duplicate check failed:" << chk.lastError().text();
        m_db.rollback();
        client->write("S[ERROR][Database query error]\n");
        return false;
    }
    if (chk.value(0).toInt() > 0) {
        m_db.rollback();
        client->write("S[ERROR][Username or email already exists]\n");
        return false;
    }

    QSqlQuery ins(m_db);
    const QString sql = R"(
      INSERT INTO members
        (firstname, lastname, email, phone, username, password)
      VALUES
        (:f, :l, :e, :ph, :u, :p)
    )";
    ins.prepare(sql);
    ins.bindValue(":f", firstname);
    ins.bindValue(":l", lastname);
    ins.bindValue(":e", email);
    ins.bindValue(":ph", phone);
    ins.bindValue(":u", username);
    ins.bindValue(":p", password);

    if (!ins.exec()) {
        qWarning() << "MemberDB: insert failed: " << ins.lastError().text();
        m_db.rollback();
        client->write("S[ERROR][Database insert failed]\n");
        return false;
    }

    m_db.commit();
    client->write("S[OK][Signup successful]\n");
    qDebug() << "MemberDB: Member added:" << username;
    return true;
}

bool MemberDatabaseManager::updateMemberAllFields(QTcpSocket* socket,
                                                  const QString &oldUsername,
                                                  const QString &firstname,
                                                  const QString &lastname,
                                                  const QString &phone,
                                                  const QString &email,
                                                  const QString &newUsername,
                                                  const QString &password)
{
    if (!socket) {
        qWarning() << "MemberDB: Null socket in updateMemberAllFields";
        return false;
    }
    if (!m_db.isOpen()) {
        qWarning() << "MemberDB: updateMemberAllFields called but DB is not open!";
        socket->write("C[CF][ERROR][Database is not open]\n");
        return false;
    }

    m_db.transaction();

    QSqlQuery q(m_db);
    q.prepare("SELECT password FROM members WHERE username = :u;");
    q.bindValue(":u", oldUsername);
    if (!q.exec() || !q.next()) {
        qWarning() << "MemberDB: user not found:" << oldUsername;
        m_db.rollback();
        socket->write("C[CF][ERROR][User not found]\n");
        return false;
    }

    QString storedPassword = q.value(0).toString();
    if (storedPassword != password) {
        qWarning() << "MemberDB: password incorrect for user:" << oldUsername;
        m_db.rollback();
        socket->write("C[CF][WRONG][Incorrect password]\n");
        return false;
    }

    if (newUsername != oldUsername || true) { // Assume email might change
        QSqlQuery dup(m_db);
        dup.prepare(R"(
          SELECT COUNT(*) FROM members
          WHERE (username = :newU OR email = :newE)
            AND username != :oldU;
        )");
        dup.bindValue(":newU", newUsername);
        dup.bindValue(":newE", email);
        dup.bindValue(":oldU", oldUsername);
        if (!dup.exec() || !dup.next()) {
            qWarning() << "MemberDB: duplicate-check failed:" << dup.lastError().text();
            m_db.rollback();
            socket->write("C[CF][ERROR][Database query error]\n");
            return false;
        }
        if (dup.value(0).toInt() > 0) {
            m_db.rollback();
            socket->write("C[CF][ERROR][Username or email already exists]\n");
            return false;
        }
    }

    QSqlQuery up(m_db);
    up.prepare(R"(
        UPDATE members SET
          firstname = :firstname,
          lastname  = :lastname,
          phone     = :phone,
          email     = :email,
          username  = :newUsername,
          password  = :newPassword
        WHERE username = :oldUsername;
    )");
    up.bindValue(":firstname", firstname);
    up.bindValue(":lastname", lastname);
    up.bindValue(":phone", phone);
    up.bindValue(":email", email);
    up.bindValue(":newUsername", newUsername);
    up.bindValue(":newPassword", password);
    up.bindValue(":oldUsername", oldUsername);

    if (!up.exec()) {
        qWarning() << "MemberDB: update failed:" << up.lastError().text();
        m_db.rollback();
        socket->write("C[CF][ERROR][Profile update failed]\n");
        return false;
    }

    if (up.numRowsAffected() > 0) {
        m_db.commit();
        socket->write("C[CF][OK][Profile updated][Your profile was updated successfully]\n");
        return true;
    } else {
        m_db.rollback();
        socket->write("C[CF][INFO][No changes][No information was changed in your profile]\n");
        return false;
    }
}

QVariantMap MemberDatabaseManager::fetchMemberBy(const QString &column, const QVariant &value)
{
    QVariantMap m;
    if (!m_db.isOpen())
        return m;

    QString stmt = QString(R"(
        SELECT firstname, lastname,
               email, phone,
               username, password
        FROM members
        WHERE %1 = :val;
    )").arg(column);

    QSqlQuery q(m_db);
    q.prepare(stmt);
    q.bindValue(":val", value);
    if (q.exec() && q.next()) {
        m["firstname"] = q.value("firstname").toString();
        m["lastname"] = q.value("lastname").toString();
        m["email"] = q.value("email").toString();
        m["phone"] = q.value("phone").toString();
        m["username"] = q.value("username").toString();
        m["password"] = q.value("password").toString();
    }
    return m;
}

QVariantMap MemberDatabaseManager::GetMemberByUsername(const QString &username)
{
    return fetchMemberBy("username", username);
}

QVariantMap MemberDatabaseManager::GetMemberByEmail(const QString &email)
{
    return fetchMemberBy("email", email);
}

QVariantMap MemberDatabaseManager::GetMemberByPhone(const QString &phone)
{
    return fetchMemberBy("phone", phone);
}

QList<QVariantMap> MemberDatabaseManager::getAllMembers()
{
    QList<QVariantMap> list;
    if (!m_db.isOpen())
        return list;

    QSqlQuery q(m_db);
    const QString stmt = R"(
      SELECT firstname, lastname,
             email, phone,
             username, password
      FROM members;
    )";
    if (q.exec(stmt)) {
        while (q.next()) {
            QVariantMap m;
            m["firstname"] = q.value("firstname").toString();
            m["lastname"] = q.value("lastname").toString();
            m["email"] = q.value("email").toString();
            m["phone"] = q.value("phone").toString();
            m["username"] = q.value("username").toString();
            m["password"] = q.value("password").toString();
            list.append(m);
        }
    }
    return list;
}
