#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QHostAddress>
#include "memberdatabasemanager.h"

class PreGame;

class SERVER : public QObject
{
    Q_OBJECT

    QTcpServer*                 server;
    QMap<QTcpSocket*, QString>  clients;
    int                         total;
    MemberDatabaseManager&      db;
    PreGame*                    lobby;

public:
    explicit SERVER(MemberDatabaseManager& db,
                    const QString& address,
                    int port = 8888,
                    QObject* parent = nullptr);

private slots:
    QStringList extractFields(const QString &s) const;

    // Command handlers
    void handleLogin(QTcpSocket *client, const QStringList &f);
    void handleSignup(QTcpSocket *client, const QStringList &f);
    void handleRecover(QTcpSocket *client, const QStringList &f);
    void handleUpdateProfile(QTcpSocket *client, const QStringList &f);
    void handlePreGame(QTcpSocket *client, const QStringList &f);
    void handleUnknown(QTcpSocket *client);

    void OnNewConnection();
    void OnReadyRead();
    void OnClientDisconnection(QTcpSocket* client);
};

#endif // SERVER_H
