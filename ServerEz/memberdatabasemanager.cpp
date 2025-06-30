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
bool MemberDatabaseManager::updateMemberAllFields(QTcpSocket *socket, const QString &oldUsername, const QString &firstname, const QString &lastname, const QString &phone, const QString &email, const QString &newUsername, const QString &newPassword)
{
    // بررسی وجود نام کاربری قدیمی در دیتابیس
    QVariantMap existingMember = GetMemberByUsername(oldUsername);
    if (existingMember.isEmpty()) {
        qDebug() << "Old username not found in database during update:" << oldUsername;
        QString response = QString("S[CF][NOTFOUND][Old username not found in database.]");
        socket->write(response.toUtf8() + "\n");
        socket->flush();
        return false;
    }

    // ذخیره اطلاعات قدیمی برای بازگرداندن در صورت نیاز
    QVariantMap oldMemberData = existingMember;

    // حذف ردیف قدیمی از دیتابیس
    QSqlQuery deleteQuery(m_db);
    deleteQuery.prepare("DELETE FROM members WHERE username = :username");
    deleteQuery.bindValue(":username", oldUsername);
    if (!deleteQuery.exec()) {
        qDebug() << "Failed to delete old member data for username:" << oldUsername << "Error:" << deleteQuery.lastError().text();
        QString response = QString("S[CF][ERROR][Failed to delete old member data.]");
        socket->write(response.toUtf8() + "\n");
        socket->flush();
        return false;
    }

    // بررسی وجود نام کاربری جدید در دیتابیس (بعد از حذف ردیف قدیمی)
    QVariantMap duplicateMember = GetMemberByUsername(newUsername);

    if (!duplicateMember.isEmpty()) {
        // اگر نام کاربری جدید وجود دارد، ردیف قدیمی را بازمی‌گردانیم
        qDebug() << "New username already exists during update:" << newUsername;
        QSqlQuery restoreQuery(m_db);
        restoreQuery.prepare("INSERT INTO members (username, firstname, lastname, email, phone, password) "
                             "VALUES (:username, :firstname, :lastname, :email, :phone, :password)");
        restoreQuery.bindValue(":username", oldMemberData["username"].toString());
        restoreQuery.bindValue(":firstname", oldMemberData["firstname"].toString());
        restoreQuery.bindValue(":lastname", oldMemberData["lastname"].toString());
        restoreQuery.bindValue(":email", oldMemberData["email"].toString());
        restoreQuery.bindValue(":phone", oldMemberData["phone"].toString());
        restoreQuery.bindValue(":password", oldMemberData["password"].toString());
        if (!restoreQuery.exec()) {
            qDebug() << "Failed to restore old member data for username:" << oldUsername << "Error:" << restoreQuery.lastError().text();
            QString response = QString("S[CF][ERROR][Critical error: Failed to restore old data.]");
            socket->write(response.toUtf8() + "\n");
            socket->flush();
            return false;
        }
        QString response = QString("S[CF][DUPLICATE][New username already exists.]");
        socket->write(response.toUtf8() + "\n");
        socket->flush();
        return false;
    }

    // اگر نام کاربری جدید وجود ندارد، ردیف جدید با اطلاعات به‌روزرسانی‌شده ایجاد می‌شود
    QSqlQuery insertQuery(m_db);
    insertQuery.prepare("INSERT INTO members (username, firstname, lastname, email, phone, password) "
                        "VALUES (:username, :firstname, :lastname, :email, :phone, :password)");
    insertQuery.bindValue(":username", newUsername);
    insertQuery.bindValue(":firstname", firstname);
    insertQuery.bindValue(":lastname", lastname);
    insertQuery.bindValue(":email", email);
    insertQuery.bindValue(":phone", phone);
    insertQuery.bindValue(":password", newPassword); // فرض می‌کنیم رمز عبور به صورت هش‌شده ذخیره می‌شود

    if (!insertQuery.exec()) {
        // در صورت عدم موفقیت در درج اطلاعات جدید، ردیف قدیمی را بازمی‌گردانیم
        qDebug() << "Failed to insert updated member data for username:" << newUsername << "Error:" << insertQuery.lastError().text();
        QSqlQuery restoreQuery(m_db);
        restoreQuery.prepare("INSERT INTO members (username, firstname, lastname, email, phone, password) "
                             "VALUES (:username, :firstname, :lastname, :email, :phone, :password)");
        restoreQuery.bindValue(":username", oldMemberData["username"].toString());
        restoreQuery.bindValue(":firstname", oldMemberData["firstname"].toString());
        restoreQuery.bindValue(":lastname", oldMemberData["lastname"].toString());
        restoreQuery.bindValue(":email", oldMemberData["email"].toString());
        restoreQuery.bindValue(":phone", oldMemberData["phone"].toString());
        restoreQuery.bindValue(":password", oldMemberData["password"].toString());
        if (!restoreQuery.exec()) {
            qDebug() << "Failed to restore old member data for username:" << oldUsername << "Error:" << restoreQuery.lastError().text();
            QString response = QString("S[CF][ERROR][Critical error: Failed to restore old data after failed update.]");
            socket->write(response.toUtf8() + "\n");
            socket->flush();
            return false;
        }
        QString response = QString("S[CF][ERROR][Failed to insert updated member data.]");
        socket->write(response.toUtf8() + "\n");
        socket->flush();
        return false;
    }

    // اگر همه چیز با موفقیت انجام شد
    qDebug() << "Member data updated successfully from username:" << oldUsername << "to new username:" << newUsername;

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
