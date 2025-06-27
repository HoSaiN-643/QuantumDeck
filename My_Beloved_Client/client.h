#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QHostAddress>

class QTcpSocket;
class Player;

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    ~Client();

    // این متد دقیقاً اسمش مثل شماست
    void ConnectToServer(const QHostAddress &host, quint16 port);
    void DisconnectFromServer();
    void WriteToServer(const QString& data);

signals:
    void ConnectedToServer();
    void DisconnectedFromServer();
    void ErrorOccurred(const QString &errorString);

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);
    void OnReadyRead();

private:
    QTcpSocket *m_socket;
    Player* player;


};

#endif // CLIENT_H
