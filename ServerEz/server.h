#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QMap>
#include "pregame.h"
#include "memberdatabasemanager.h"
#include "game.h"

class SERVER : public QTcpServer
{
    Q_OBJECT

public:
    explicit SERVER(MemberDatabaseManager* dbManager, const QString& address, QObject *parent = nullptr);
    ~SERVER();

    void setIP(const QString &address);
    void setPort(quint16 port);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnection();
    void onStartGame(const QStringList &players);

private:
    void handleLogin(QTcpSocket *client, const QStringList &fields);
    void handleSignup(QTcpSocket *client, const QStringList &fields);
    void handleRecover(QTcpSocket *client, const QStringList &fields);
    void handlePreGame(QTcpSocket *client, const QStringList &fields);
    void handleUpdateProfile(QTcpSocket *client, const QStringList &fields);
    void handleUnknown(QTcpSocket *client);
    QStringList extractFields(const QString &payload) const;

    MemberDatabaseManager* dbManager;
    QString address;
    quint16 port;
    QMap<QTcpSocket*, QString> clients;
    PreGame* preGame;
    Game* game;
};

#endif // SERVER_H
