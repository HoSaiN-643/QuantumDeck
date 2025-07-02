#include "memberdatabasemanager.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QRegularExpression>

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
            lastname TEXT NOT NULL,
            email TEXT UNIQUE NOT NULL,
            phone TEXT,
            username TEXT UNIQUE NOT NULL,
            password TEXT NOT NULL
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
        client->flush();
        return false;
    }

    // بررسی ساده فرمت ایمیل


    // بررسی خالی نبودن فیلدها
    if (firstname.isEmpty() || lastname.isEmpty() || username.isEmpty() || password.isEmpty()) {
        client->write("S[ERROR][Required fields cannot be empty]\n");
        client->flush();
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
        client->flush();
        return false;
    }
    if (chk.value(0).toInt() > 0) {
        m_db.rollback();
        client->write("S[ERROR][Username or email already exists]\n");
        client->flush();
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
    ins.bindValue(":p", password); // هشدار: رمز عبور باید هش شود (برای آینده)

    if (!ins.exec()) {
        qWarning() << "MemberDB: insert failed: " << ins.lastError().text();
        m_db.rollback();
        client->write("S[ERROR][Database insert failed]\n");
        client->flush();
        return false;
    }

    m_db.commit();
    client->write("S[OK][Signup successful]\n");
    client->flush();
    qDebug() << "MemberDB: Member added:" << username;
    return true;
}

bool MemberDatabaseManager::updateMemberAllFields(QTcpSocket *socket, const QString &oldUsername,
                                                  const QString &firstname, const QString &lastname,
                                                  const QString &phone, const QString &email,
                                                  const QString &newUsername, const QString &newPassword)
{
    if (!socket) {
        qWarning() << "MemberDB: Null socket in updateMemberAllFields";
        return false;
    }

    // بررسی وجود نام کاربری قدیمی
    QVariantMap existingMember = GetMemberByUsername(oldUsername);
    if (existingMember.isEmpty()) {
        qDebug() << "Old username not found in database during update:" << oldUsername;
        socket->write("C[CF][NOTFOUND][Old username not found in database.]\n");
        socket->flush();
        return false;
    }

    // بررسی فرمت ایمیل


    // بررسی خالی نبودن فیلدها
    if (firstname.isEmpty() || lastname.isEmpty() || newUsername.isEmpty() || newPassword.isEmpty()) {
        socket->write("C[CF][ERROR][Required fields cannot be empty]\n");
        socket->flush();
        return false;
    }

    // بررسی وجود نام کاربری جدید
    if (newUsername != oldUsername) {
        QVariantMap duplicateMember = GetMemberByUsername(newUsername);
        if (!duplicateMember.isEmpty()) {
            qDebug() << "New username already exists during update:" << newUsername;
            socket->write("C[CF][DUPLICATE][New username already exists.]\n");
            socket->flush();
            return false;
        }
    }

    // به‌روزرسانی اطلاعات
    QSqlQuery updateQuery(m_db);
    updateQuery.prepare("UPDATE members SET firstname = :firstname, lastname = :lastname, email = :email, phone = :phone, username = :username, password = :password WHERE username = :oldUsername");
    updateQuery.bindValue(":firstname", firstname);
    updateQuery.bindValue(":lastname", lastname);
    updateQuery.bindValue(":email", email);
    updateQuery.bindValue(":phone", phone);
    updateQuery.bindValue(":username", newUsername);
    updateQuery.bindValue(":password", newPassword);
    updateQuery.bindValue(":oldUsername", oldUsername);

    if (!updateQuery.exec()) {
        qDebug() << "Failed to update member data for username:" << oldUsername << "Error:" << updateQuery.lastError().text();
        socket->write(QString("C[CF][ERROR][Failed to update member data: %1]\n").arg(updateQuery.lastError().text()).toUtf8());
        socket->flush();
        return false;
    }

    qDebug() << "Member data updated successfully from username:" << oldUsername << "to new username:" << newUsername;
    socket->write(QString("C[CF][OK][Information updated successfully][%1][%2][%3][%4][%5][%6]\n")
                      .arg(firstname, lastname, phone, email, newUsername, newPassword).toUtf8());
    socket->flush();
    return true;
}

QVariantMap MemberDatabaseManager::fetchMemberBy(const QString &column, const QVariant &value)
{
    QVariantMap m;
    if (!m_db.isOpen()) return m;

    QString stmt = QString(R"(
        SELECT firstname, lastname, email, phone, username, password
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
    if (!m_db.isOpen()) return list;

    QSqlQuery q(m_db);
    const QString stmt = R"(
        SELECT firstname, lastname, email, phone, username, password
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
