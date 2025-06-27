#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QHostAddress>

class QTcpSocket;

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    ~Client();

    // این متد دقیقاً اسمش مثل شماست
    void ConnectToServer(const QHostAddress &host, quint16 port);
    void DisconnectFromServer();

signals:
    void ConnectedToServer();
    void DisconnectedFromServer();
    void ErrorOccurred(const QString &errorString);

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *m_socket;
};

#endif // CLIENT_H
