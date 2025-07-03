#include "memberdatabasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

MemberDatabaseManager::MemberDatabaseManager(const QString &dbPath, QObject *parent)
    : QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE", "members_db");
    db.setDatabaseName(dbPath);
    if (!db.open()) {
        qDebug() << "Cannot open database:" << db.lastError().text();
        return;
    }

    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS members ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "firstName TEXT NOT NULL, "
               "lastName TEXT NOT NULL, "
               "phone TEXT NOT NULL, "
               "email TEXT NOT NULL UNIQUE, "
               "username TEXT NOT NULL UNIQUE, "
               "password TEXT NOT NULL)");
    query.exec("CREATE TABLE IF NOT EXISTS game_history ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "username TEXT NOT NULL, "
               "player1 TEXT NOT NULL, "
               "player2 TEXT NOT NULL, "
               "winner TEXT NOT NULL, "
               "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)");

    // اضافه کردن کاربر تست
    if (!isUsernameTaken("hosein_643")) {
        addMember("Hosein", "Test", "1234567890", "hosein@example.com", "hosein_643", "Aot643710");
        qDebug() << "Test user 'hosein_643' added to database.";
    }
}

MemberDatabaseManager::~MemberDatabaseManager()
{
    if (db.isOpen()) {
        db.close();
    }
    QSqlDatabase::removeDatabase("members_db");
}

QString MemberDatabaseManager::getUsernameFromEmail(const QString &email)
{
    QSqlQuery query(db);
    query.prepare("SELECT username FROM members WHERE email = :email");
    query.bindValue(":email", email);
    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return QString();
}

QString MemberDatabaseManager::getPasswordFromPhone(const QString &phone)
{
    QSqlQuery query(db);
    query.prepare("SELECT password FROM members WHERE phone = :phone");
    query.bindValue(":phone", phone);
    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return QString();
}

bool MemberDatabaseManager::isValidSignup(const QString &firstName, const QString &lastName,
                                          const QString &phone, const QString &email,
                                          const QString &username, const QString &password)
{
    return !firstName.isEmpty() && !lastName.isEmpty() && !phone.isEmpty() &&
           !email.isEmpty() && !username.isEmpty() && !password.isEmpty();
}

bool MemberDatabaseManager::addMember(const QString &firstName, const QString &lastName,
                                      const QString &phone, const QString &email,
                                      const QString &username, const QString &password)
{
    if (isUsernameTaken(username) || isEmailTaken(email)) {
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO members (firstName, lastName, phone, email, username, password) "
                  "VALUES (:firstName, :lastName, :phone, :email, :username, :password)");
    query.bindValue(":firstName", firstName);
    query.bindValue(":lastName", lastName);
    query.bindValue(":phone", phone);
    query.bindValue(":email", email);
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (query.exec()) {
        return true;
    }
    qDebug() << "Add member failed:" << query.lastError().text();
    return false;
}

bool MemberDatabaseManager::updateMemberAllFields(const QString &oldUsername, const QString &firstName,
                                                  const QString &lastName, const QString &phone,
                                                  const QString &email, const QString &newUsername,
                                                  const QString &password)
{
    if (oldUsername != newUsername && isUsernameTaken(newUsername)) {
        return false;
    }
    if (isEmailTaken(email)) {
        QSqlQuery checkEmail(db);
        checkEmail.prepare("SELECT username FROM members WHERE email = :email");
        checkEmail.bindValue(":email", email);
        checkEmail.exec();
        if (checkEmail.next() && checkEmail.value(0).toString() != oldUsername) {
            return false;
        }
    }

    QSqlQuery query(db);
    query.prepare("UPDATE members SET firstName = :firstName, lastName = :lastName, "
                  "phone = :phone, email = :email, username = :newUsername, password = :password "
                  "WHERE username = :oldUsername");
    query.bindValue(":firstName", firstName);
    query.bindValue(":lastName", lastName);
    query.bindValue(":phone", phone);
    query.bindValue(":email", email);
    query.bindValue(":newUsername", newUsername);
    query.bindValue(":password", password);
    query.bindValue(":oldUsername", oldUsername);

    if (query.exec()) {
        return true;
    }
    qDebug() << "Update member failed:" << query.lastError().text();
    return false;
}

bool MemberDatabaseManager::isUsernameTaken(const QString &username)
{
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM members WHERE username = :username");
    query.bindValue(":username", username);
    query.exec();
    query.next();
    return query.value(0).toInt() > 0;
}

bool MemberDatabaseManager::isEmailTaken(const QString &email)
{
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM members WHERE email = :email");
    query.bindValue(":email", email);
    query.exec();
    query.next();
    return query.value(0).toInt() > 0;
}

QVariantMap MemberDatabaseManager::getMemberInfo(const QString &username)
{
    QVariantMap info;
    QSqlQuery query(db);
    query.prepare("SELECT firstName, lastName, phone, email, username, password "
                  "FROM members WHERE username = :username");
    query.bindValue(":username", username);
    if (query.exec() && query.next()) {
        info["firstName"] = query.value("firstName").toString();
        info["lastName"] = query.value("lastName").toString();
        info["phone"] = query.value("phone").toString();
        info["email"] = query.value("email").toString();
        info["username"] = query.value("username").toString();
        info["password"] = query.value("password").toString();
    }
    return info;
}

QList<QVariantMap> MemberDatabaseManager::getGameHistory(const QString &username)
{
    QList<QVariantMap> history;
    QSqlQuery query(db);
    query.prepare("SELECT player1, player2, winner FROM game_history WHERE username = :username");
    query.bindValue(":username", username);
    if (query.exec()) {
        while (query.next()) {
            QVariantMap game;
            game["player1"] = query.value("player1").toString();
            game["player2"] = query.value("player2").toString();
            game["winner"] = query.value("winner").toString();
            history.append(game);
        }
    } else {
        qDebug() << "Get game history failed:" << query.lastError().text();
    }
    return history;
}

bool MemberDatabaseManager::addGameHistory(const QString &username, const QString &player1,
                                           const QString &player2, const QString &winner)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO game_history (username, player1, player2, winner) "
                  "VALUES (:username, :player1, :player2, :winner)");
    query.bindValue(":username", username);
    query.bindValue(":player1", player1);
    query.bindValue(":player2", player2);
    query.bindValue(":winner", winner);

    if (query.exec()) {
        return true;
    }
    qDebug() << "Add game history failed:" << query.lastError().text();
    return false;
}
