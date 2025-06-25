#include "memberdatabasemanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

// ctor
MemberDatabaseManager::MemberDatabaseManager(const QString &dbPath)
    : m_dbPath(dbPath),
    m_db(QSqlDatabase::addDatabase("QSQLITE"))
{
}

// dtor
MemberDatabaseManager::~MemberDatabaseManager()
{
    close();
}

// open db
bool MemberDatabaseManager::open()
{
    m_db.setDatabaseName(m_dbPath);
    if (!m_db.open()) {
        qWarning() << "Cannot open database:" << m_db.lastError().text();
        return false;
    }
    qDebug() << "Database opened:" << m_dbPath;
    return true;
}

// close db
void MemberDatabaseManager::close()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    qDebug() << "Database closed.";
}

// بررسی وجود ستون در جدول
bool MemberDatabaseManager::columnExists(const QString &tableName, const QString &columnName)
{
    QSqlQuery q(m_db);
    q.exec(QString("PRAGMA table_info(%1)").arg(tableName));
    while (q.next()) {
        if (q.value("name").toString() == columnName)
            return true;
    }
    return false;
}

bool MemberDatabaseManager::createMemberTable()
{
    QSqlQuery query(m_db);

    const QString stmt =
        "CREATE TABLE IF NOT EXISTS members ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  username TEXT UNIQUE NOT NULL,"
        "  email    TEXT UNIQUE NOT NULL,"
        "  password TEXT NOT NULL"
        ");";
    if (!query.exec(stmt)) {
        qWarning() << "Failed to create base table:" << query.lastError().text();
        return false;
    }


    struct Col { QString name, def; };
    QList<Col> extras = {
        { "firstname", "TEXT" },
        { "lastname",  "TEXT" },
        { "phone",     "TEXT" }
    };


    for (auto &c : extras) {
        if (!columnExists("members", c.name)) {
            QString sql = QString("ALTER TABLE members ADD COLUMN %1 %2").arg(c.name, c.def);
            if (!query.exec(sql)) {
                qWarning() << "Failed to add column" << c.name << ":" << query.lastError().text();
                return false;
            }
            qDebug() << "Added column" << c.name;
        }
    }

    qDebug() << "Table 'members' is ready/up-to-date.";
    return true;
}


bool MemberDatabaseManager::addMember(QTcpSocket* client,
                                      const QString &username,
                                      const QString &email,
                                      const QString &password,
                                      const QString &firstname,
                                      const QString &lastname,
                                      const QString &phone)
{
    if (!m_db.isOpen()) {
        qWarning() << "Database is not open!";
        return false;
    }


    {
        QSqlQuery chk(m_db);
        chk.prepare("SELECT COUNT(*) FROM members WHERE username = :u OR email = :e");
        chk.bindValue(":u", username);
        chk.bindValue(":e", email);
        if (!chk.exec() || !chk.next()) {
            qWarning() << "Check existing failed:" << chk.lastError().text();
            client->write("S[FAIL][Check existing failed.]");
            return false;
        }
        if (chk.value(0).toInt() > 0) {
            client->write("S[FAIL][Username or email already exists.]");
            return false;
        }
    }


    QSqlQuery ins(m_db);
    ins.prepare(R"(
        INSERT INTO members
        (username, email, password, firstname, lastname, phone)
        VALUES(:u, :e, :p, :f, :l, :ph)
    )");
    ins.bindValue(":u",  username);
    ins.bindValue(":e",  email);
    ins.bindValue(":p",  password);
    ins.bindValue(":f",  firstname);
    ins.bindValue(":l",  lastname);
    ins.bindValue(":ph", phone);

    if (!ins.exec()) {
        qWarning() << "Insert failed:" << ins.lastError().text();
        client->write("S[FAIL][Insert failed.]");
        return false;
    }

    client->write("S[OK][Successfully signed up]");
    qDebug() << "Member added:" << username;
    return true;
}

// حذف عضو
bool MemberDatabaseManager::removeMemberIfCredentialsMatch(
    const QString &username, const QString &password)
{
    if (!m_db.isOpen()) return false;

    QSqlQuery del(m_db);
    del.prepare("DELETE FROM members WHERE username = :u AND password = :p");
    del.bindValue(":u", username);
    del.bindValue(":p", password);
    if (!del.exec()) {
        qWarning() << "Delete failed:" << del.lastError().text();
        return false;
    }
    return del.numRowsAffected() > 0;
}


bool MemberDatabaseManager::updateMemberField(int fieldType,
                                              const QString &currentUsername,
                                              const QString &newValue)
{
    if (!m_db.isOpen()) return false;

    QString column;
    switch (fieldType) {
    case 1: column = "username"; break;
    case 2: column = "email";    break;
    case 3: column = "password"; break;
    case 4: column = "firstname";break;
    case 5: column = "lastname"; break;
    case 6: column = "phone";    break;
    default:
        return false;
    }

    QSqlQuery q(m_db);
    q.prepare(QString("UPDATE members SET %1 = :val WHERE username = :u").arg(column));
    q.bindValue(":val", newValue);
    q.bindValue(":u",   currentUsername);

    if (!q.exec()) {
        qWarning() << "Update failed:" << q.lastError().text();
        return false;
    }
    return q.numRowsAffected() > 0;
}


QVariantMap MemberDatabaseManager::GetMemberByUsername(const QString &username)
{
    QVariantMap m;
    if (!m_db.isOpen()) return m;

    QSqlQuery q(m_db);
    q.prepare(R"(
      SELECT id, username, email, password,
             firstname, lastname, phone
      FROM members WHERE username = :u
    )");
    q.bindValue(":u", username);
    if (q.exec() && q.next()) {
        m["id"]        = q.value("id").toInt();
        m["username"]  = q.value("username").toString();
        m["email"]     = q.value("email").toString();
        m["password"]  = q.value("password").toString();
        m["firstname"] = q.value("firstname").toString();
        m["lastname"]  = q.value("lastname").toString();
        m["phone"]     = q.value("phone").toString();
    }
    return m;
}


QVariantMap MemberDatabaseManager::GetMemberByEmail(const QString &email)
{
    QVariantMap m;
    if (!m_db.isOpen()) return m;

    QSqlQuery q(m_db);
    q.prepare(R"(
      SELECT id, username, email, password,
             firstname, lastname, phone
      FROM members WHERE email = :e
    )");
    q.bindValue(":e", email);
    if (q.exec() && q.next()) {
        m["id"]        = q.value("id").toInt();
        m["username"]  = q.value("username").toString();
        m["email"]     = q.value("email").toString();
        m["password"]  = q.value("password").toString();
        m["firstname"] = q.value("firstname").toString();
        m["lastname"]  = q.value("lastname").toString();
        m["phone"]     = q.value("phone").toString();
    }
    return m;
}
QVariantMap MemberDatabaseManager::GetMemberByPhone(const QString &phone)
{
    QVariantMap m;
    if (!m_db.isOpen()) return m;

    QSqlQuery q(m_db);
    q.prepare(R"(
      SELECT id, username, email, password,
             firstname, lastname, phone
      FROM members WHERE phone = :ph
    )");
    q.bindValue(":ph", phone);
    if (q.exec() && q.next()) {
        m["id"]        = q.value("id").toInt();
        m["username"]  = q.value("username").toString();
        m["email"]     = q.value("email").toString();
        m["password"]  = q.value("password").toString();
        m["firstname"] = q.value("firstname").toString();
        m["lastname"]  = q.value("lastname").toString();
        m["phone"]     = q.value("phone").toString();
    }
    return m;
}


QList<QVariantMap> MemberDatabaseManager::getAllMembers()
{
    QList<QVariantMap> list;
    if (!m_db.isOpen()) return list;

    QSqlQuery q(m_db);
    q.prepare(R"(
      SELECT id, username, email, password,
             firstname, lastname, phone
      FROM members
    )");
    if (q.exec()) {
        while (q.next()) {
            QVariantMap m;
            m["id"]        = q.value("id").toInt();
            m["username"]  = q.value("username").toString();
            m["email"]     = q.value("email").toString();
            m["password"]  = q.value("password").toString();
            m["firstname"] = q.value("firstname").toString();
            m["lastname"]  = q.value("lastname").toString();
            m["phone"]     = q.value("phone").toString();
            list.append(m);
        }
    }
    return list;
}
