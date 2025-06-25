#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QHostAddress>
#include <memberdatabasemanager.h>

class SERVER : public QObject
{
    Q_OBJECT

    QTcpServer*                 server;
    QMap<QTcpSocket*,QString>   clients;
    int                         total;
    MemberDatabaseManager&      db;

public:
    explicit SERVER(MemberDatabaseManager& db,
                    const QString& address,
                    int port = 8888,
                    QObject* parent = nullptr);

private slots:
    void OnNewConnection();
    void OnReadyRead();
    void OnClientDisconnection(QTcpSocket* client);
};

#endif // SERVER_H
