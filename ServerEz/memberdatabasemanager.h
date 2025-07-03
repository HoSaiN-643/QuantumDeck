#ifndef MEMBERDATABASEMANAGER_H
#define MEMBERDATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QTcpSocket>

class MemberDatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit MemberDatabaseManager(const QString &dbPath, QObject *parent = nullptr);
    ~MemberDatabaseManager();

    QString getUsernameFromEmail(const QString &email);
    QString getPasswordFromPhone(const QString &phone);
    bool isValidSignup(const QString &firstName, const QString &lastName, const QString &phone,
                       const QString &email, const QString &username, const QString &password);
    bool addMember(const QString &firstName, const QString &lastName, const QString &phone,
                   const QString &email, const QString &username, const QString &password);
    bool updateMemberAllFields(const QString &oldUsername, const QString &firstName,
                               const QString &lastName, const QString &phone, const QString &email,
                               const QString &newUsername, const QString &password);
    bool isUsernameTaken(const QString &username);
    bool isEmailTaken(const QString &email);
    QVariantMap getMemberInfo(const QString &username);
    QList<QVariantMap> getGameHistory(const QString &username);
    bool addGameHistory(const QString &username, const QString &player1, const QString &player2, const QString &winner);

private:
    QSqlDatabase db;
};

#endif // MEMBERDATABASEMANAGER_H
