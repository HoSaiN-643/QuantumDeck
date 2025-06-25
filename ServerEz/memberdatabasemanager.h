#ifndef MEMBERDATABASEMANAGER_H
#define MEMBERDATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QList>
#include <QVariantMap>
#include <QTcpSocket>

class MemberDatabaseManager
{
public:
    explicit MemberDatabaseManager(const QString &dbPath);
    ~MemberDatabaseManager();

    bool open();
    void close();

    bool createMemberTable();

    bool addMember(QTcpSocket* client,
                   const QString &username,
                   const QString &email,
                   const QString &password,
                   const QString &firstname,
                   const QString &lastname,
                   const QString &phone);


    bool removeMemberIfCredentialsMatch(const QString &username,
                                        const QString &password);


    bool updateMemberField(int fieldType,
                           const QString &currentUsername,
                           const QString &newValue);


    QVariantMap GetMemberByUsername(const QString &username);
    QVariantMap GetMemberByEmail(const QString &email);

    QList<QVariantMap> getAllMembers();

    QVariantMap GetMemberByPhone(const QString &phone);
private:
    QString      m_dbPath;
    QSqlDatabase m_db;

    bool columnExists(const QString &tableName, const QString &columnName);
};

#endif // MEMBERDATABASEMANAGER_H
