#ifndef MEMBERDATABASEMANAGER_H
#define MEMBERDATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QVariantMap>
#include <QList>
#include <QTcpSocket>

class MemberDatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit MemberDatabaseManager(const QString &dbPath, QObject *parent = nullptr);
    ~MemberDatabaseManager();

    bool open();
    void close();

    bool columnExists(const QString &tableName, const QString &columnName);
    bool createMemberTable();

    bool removeMemberIfCredentialsMatch(const QString &username,
                                        const QString &password);

    bool updateMemberAllFields(QTcpSocket* socket,
                               const QString &oldUsername,
                               const QString &firstname,
                               const QString &lastname,
                               const QString &email,
                               const QString &phone,
                               const QString &newUsername,
                               const QString &password);

    QVariantMap GetMemberByUsername(const QString &username);
    QVariantMap GetMemberByEmail(const QString &email);
    QVariantMap GetMemberByPhone(const QString &phone);

    QList<QVariantMap> getAllMembers();

    // امضای جدید مطابق درخواستی که شما گفتید:
    // (client, firstname, lastname, email, phone, username, password)
    bool addMember(QTcpSocket *client,
                   const QString &firstname,
                   const QString &lastname,
                   const QString &email,
                   const QString &phone,
                   const QString &username,
                   const QString &password);

private:
    QVariantMap fetchMemberBy(const QString &column, const QVariant &value);

private:
    QString       m_dbPath;
    QString       m_connectionName;
    QSqlDatabase  m_db;
};

#endif // MEMBERDATABASEMANAGER_H
