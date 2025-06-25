#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <log.h>

class Client : public QObject
{
    Q_OBJECT

    QTcpSocket* socket;
    Log logUi;

public:
    explicit Client(QObject* parent = nullptr);
    void ConnectToServer(const QHostAddress& address, int port);

private slots:
    void onConnected();
    void onError(QAbstractSocket::SocketError socketError);

signals:
    void ConnectedToServer();
};

#endif // CLIENT_H
