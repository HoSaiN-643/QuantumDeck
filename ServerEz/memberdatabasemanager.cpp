#include "memberdatabasemanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QUuid>
#include <QTcpSocket>

// ctor
MemberDatabaseManager::MemberDatabaseManager(const QString &dbPath, QObject *parent)
    : QObject(parent),
    m_dbPath(dbPath),
    m_connectionName(QUuid::createUuid().toString()),
    m_db(QSqlDatabase::addDatabase("QSQLITE", m_connectionName))
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
        qWarning() << "MemberDB: Cannot open database:" << m_db.lastError().text();
        return false;
    }
    qDebug() << "MemberDB: Database opened at" << m_dbPath;
    return true;
}

// close db
void MemberDatabaseManager::close()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
    // حذف رشتهٔ اتصال منحصربه‌فرد
    QSqlDatabase::removeDatabase(m_connectionName);
    qDebug() << "MemberDB: Database closed.";
}

// بررسی وجود ستون در جدول
bool MemberDatabaseManager::columnExists(const QString &tableName,
                                         const QString &columnName)
{
    QSqlQuery q(m_db);
    if (!q.exec(QString("PRAGMA table_info(%1);").arg(tableName))) {
        qWarning() << "MemberDB: PRAGMA failed:" << q.lastError().text();
        return false;
    }
    while (q.next()) {
        // ستون دوم (index=1) در PRAGMA table_info نام ستون را دارد
        if (q.value(1).toString().compare(columnName, Qt::CaseInsensitive) == 0)
            return true;
    }
    return false;
}

// ایجاد یا به‌روز‌رسانی جدول members
bool MemberDatabaseManager::createMemberTable()
{
    if (!m_db.isOpen()) {
        qWarning() << "MemberDB: Database is not open!";
        return false;
    }

    QSqlQuery q(m_db);
    // خود تعریف جدول با ترتیب ستون‌ها: firstname، lastname، email، phone، username، password
    const QString createStmt = R"(
        CREATE TABLE IF NOT EXISTS members (
            firstname TEXT    NOT NULL,
            lastname  TEXT    NOT NULL,
            email     TEXT    UNIQUE NOT NULL,
            phone     TEXT             ,
            username  TEXT    UNIQUE NOT NULL,
            password  TEXT    NOT NULL
        );
    )";
    if (!q.exec(createStmt)) {
        qWarning() << "MemberDB: Failed to create members table:" << q.lastError().text();
        return false;
    }

    qDebug() << "MemberDB: Table 'members' is ready with columns "
                "(firstname, lastname, email, phone, username, password).";
    return true;
}

// افزودن عضو جدید
bool MemberDatabaseManager::addMember(QTcpSocket* client,
                                      const QString &username,
                                      const QString &email,
                                      const QString &password,
                                      const QString &firstname,
                                      const QString &lastname,
                                      const QString &phone)
{
    if (!m_db.isOpen()) {
        qWarning() << "MemberDB: addMember called but DB is not open!";
        client->write("S[ERROR][Database not open]");
        return false;
    }

    // Check for duplicate username or email
    {
        QSqlQuery chk(m_db);
        chk.prepare("SELECT COUNT(*) FROM members WHERE username = :u OR email = :e;");
        chk.bindValue(":u", username);
        chk.bindValue(":e", email);
        if (!chk.exec() || !chk.next()) {
            qWarning() << "MemberDB: check existing failed:" << chk.lastError().text();
            client->write("S[ERROR][Database query error]");
            return false;
        }
        if (chk.value(0).toInt() > 0) {
            client->write("S[ERROR][Username or email already exists.]");
            return false;
        }
    }

    // Insert new member
    QSqlQuery ins(m_db);
    ins.prepare(R"(
        INSERT INTO members
          (firstname, lastname, email, phone, username, password)
        VALUES
          (:f, :l, :e, :ph, :u, :p);
    )");
    ins.bindValue(":f",  firstname);
    ins.bindValue(":l",  lastname);
    ins.bindValue(":e",  email);
    ins.bindValue(":ph", phone);
    ins.bindValue(":u",  username);
    ins.bindValue(":p",  password);

    if (!ins.exec()) {
        qWarning() << "MemberDB: insert failed:" << ins.lastError().text();
        client->write("S[ERROR][Database insert failed]");
        return false;
    }

    client->write("S[OK][Signup successful]");
    qDebug() << "MemberDB: Member added:" << username;
    return true;
}


// حذف عضو در صورت تطابق نام‌کاربری و رمز
bool MemberDatabaseManager::removeMemberIfCredentialsMatch(
    const QString &username, const QString &password)
{
    if (!m_db.isOpen()) {
        qWarning() << "MemberDB: removeMember called but DB is not open!";
        return false;
    }

    QSqlQuery del(m_db);
    del.prepare("DELETE FROM members WHERE username = :u AND password = :p;");
    del.bindValue(":u", username);
    del.bindValue(":p", password);

    if (!del.exec()) {
        qWarning() << "MemberDB: delete failed:" << del.lastError().text();
        return false;
    }
    return (del.numRowsAffected() > 0);
}

bool MemberDatabaseManager::updateMemberAllFields(QTcpSocket* socket,const QString &oldUsername, const QString &firstname,
                                                  const QString &lastname, const QString &phone,
                                                  const QString &email, const QString &newUsername, const QString &password)
{
    if (!m_db.isOpen()) {
        qWarning() << "MemberDB: updateMemberAllFields called but DB is not open!";
        socket->write("C[CF][ERROR][Database is not open]");
        return false;
    }

    // گام ۱: بررسی وجود عضو
    QSqlQuery q(m_db);
    q.prepare("SELECT password FROM members WHERE username = :u");
    q.bindValue(":u", oldUsername);
    if (!q.exec() || !q.next()) {
        qWarning() << "MemberDB: user not found:" << oldUsername;
        socket->write("C[CF][ERROR][User not found]");
        return false;
    }

    // گام ۲: اعتبارسنجی رمز عبور
    QString storedPassword = q.value(0).toString();
    if (storedPassword != password) {
        qWarning() << "MemberDB: password incorrect for user:" << oldUsername;
        socket->write("C[CF][WRONG][Incorrect password]");
        return false;
    }

    // گام ۳: آپدیت رکورد
    QSqlQuery updateQ(m_db);
    updateQ.prepare(
        "UPDATE members SET "
        "firstname = :firstname, "
        "lastname  = :lastname, "
        "phone     = :phone, "
        "email     = :email, "
        "username  = :newUsername, "
        "password  = :newPassword "
        "WHERE username = :oldUsername"
        );
    updateQ.bindValue(":firstname",  firstname);
    updateQ.bindValue(":lastname",   lastname);
    updateQ.bindValue(":phone",      phone);
    updateQ.bindValue(":email",      email);
    updateQ.bindValue(":newUsername", newUsername);
    updateQ.bindValue(":newPassword", password);
    updateQ.bindValue(":oldUsername", oldUsername);

    if (!updateQ.exec()) {
        qWarning() << "MemberDB: update failed:" << updateQ.lastError().text();
        socket->write("C[CF][ERROR][Profile update failed. Please try again later.]");
        return false;
    }

    if (updateQ.numRowsAffected() > 0) {
        socket->write("C[CF][OK][Profile updated][Your profile was updated successfully.]");
        return true;
    } else {
        // کاربر وجود داشت ولی هیچ تغییری در اطلاعات حاصل نشد
        socket->write("C[CF][INFO][No changes][No information was changed in your profile.]");
        return false;
    }
}





// واکشی عضو بر اساس username
QVariantMap MemberDatabaseManager::GetMemberByUsername(const QString &username)
{
    QVariantMap m;
    if (!m_db.isOpen()) return m;

    QSqlQuery q(m_db);
    q.prepare(R"(
      SELECT firstname, lastname,
             email, phone,
             username, password
      FROM members WHERE username = :u;
    )");
    q.bindValue(":u", username);
    if (q.exec() && q.next()) {
        m["firstname"] = q.value("firstname").toString();
        m["lastname"]  = q.value("lastname").toString();
        m["email"]     = q.value("email").toString();
        m["phone"]     = q.value("phone").toString();
        m["username"]  = q.value("username").toString();
        m["password"]  = q.value("password").toString();
    }
    return m;
}

// واکشی عضو بر اساس email
QVariantMap MemberDatabaseManager::GetMemberByEmail(const QString &email)
{
    QVariantMap m;
    if (!m_db.isOpen()) return m;

    QSqlQuery q(m_db);
    q.prepare(R"(
      SELECT firstname, lastname,
             email, phone,
             username, password
      FROM members WHERE email = :e;
    )");
    q.bindValue(":e", email);
    if (q.exec() && q.next()) {
        m["firstname"] = q.value("firstname").toString();
        m["lastname"]  = q.value("lastname").toString();
        m["email"]     = q.value("email").toString();
        m["phone"]     = q.value("phone").toString();
        m["username"]  = q.value("username").toString();
        m["password"]  = q.value("password").toString();
    }
    return m;
}

// واکشی عضو بر اساس phone
QVariantMap MemberDatabaseManager::GetMemberByPhone(const QString &phone)
{
    QVariantMap m;
    if (!m_db.isOpen()) return m;

    QSqlQuery q(m_db);
    q.prepare(R"(
      SELECT firstname, lastname,
             email, phone,
             username, password
      FROM members WHERE phone = :ph;
    )");
    q.bindValue(":ph", phone);
    if (q.exec() && q.next()) {
        m["firstname"] = q.value("firstname").toString();
        m["lastname"]  = q.value("lastname").toString();
        m["email"]     = q.value("email").toString();
        m["phone"]     = q.value("phone").toString();
        m["username"]  = q.value("username").toString();
        m["password"]  = q.value("password").toString();
    }
    return m;
}

// بازگرداندن تمامی اعضا
QList<QVariantMap> MemberDatabaseManager::getAllMembers()
{
    QList<QVariantMap> list;
    if (!m_db.isOpen()) return list;

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
            m["lastname"]  = q.value("lastname").toString();
            m["email"]     = q.value("email").toString();
            m["phone"]     = q.value("phone").toString();
            m["username"]  = q.value("username").toString();
            m["password"]  = q.value("password").toString();
            list.append(m);
        }
    }
    return list;
}
