#ifndef MEMBERDATABASEMANAGER_H
#define MEMBERDATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QVariantMap>
#include <QList>

class QTcpSocket;

class MemberDatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit MemberDatabaseManager(const QString &dbPath, QObject *parent = nullptr);
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
    QVariantMap GetMemberByPhone(const QString &phone);

    QList<QVariantMap> getAllMembers();

private:
    bool columnExists(const QString &tableName, const QString &columnName);

private:
    QString      m_dbPath;
    QString      m_connectionName;
    QSqlDatabase m_db;
};

#endif // MEMBERDATABASEMANAGER_H
