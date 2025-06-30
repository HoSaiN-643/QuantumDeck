#ifndef MEMBERDATABASEMANAGER_H
#define MEMBERDATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QTcpSocket>
#include <QVariantMap>

class MemberDatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit MemberDatabaseManager(const QString &dbPath, QObject *parent = nullptr);
    ~MemberDatabaseManager();

    bool open();
    void close();
    bool createMemberTable();

    bool addMember(QTcpSocket *client,
                   const QString &firstname,
                   const QString &lastname,
                   const QString &email,
                   const QString &phone,
                   const QString &username,
                   const QString &password);

    bool updateMemberAllFields(QTcpSocket *socket,
                               const QString &oldUsername,
                               const QString &firstname,
                               const QString &lastname,
                               const QString &phone,
                               const QString &email,
                               const QString &newUsername,
                               const QString &password);

    QVariantMap GetMemberByUsername(const QString &username);
    QVariantMap GetMemberByEmail(const QString &email);
    QVariantMap GetMemberByPhone(const QString &phone);
    QList<QVariantMap> getAllMembers();

private:
    QString m_dbPath;
    QString m_connectionName;
    QSqlDatabase m_db;

    QVariantMap fetchMemberBy(const QString &column, const QVariant &value);
};

#endif // MEMBERDATABASEMANAGER_H
