#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QHostAddress>
#include "player.h"


class QTcpSocket;
class Login;


class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    ~Client();

    void ConnectToServer(const QHostAddress &host, quint16 port);
    void DisconnectFromServer();


    Player &GetPlayer();
    void WriteToServer(const QString &data);

signals:
    void ConnectedToServer();
    void DisconnectedFromServer();
    void ErrorOccurred(const QString &errorString);

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);
    void OnReadyRead();
    void handlePreGame(const QStringList &f);
signals :
    void SuccesFullSignUp();
    void SuccesFull_LogIn();
    void PreGameSearching(int waiting, int total);
    void PreGameFound(const QStringList &opponents);
    void PreGameFull(const QString &message);

private:
     QStringList extractFields(const QString &msg);
    void handleLogin(const QStringList &fields);
    void handleSignup(const QStringList &fields);
    void handleRecover(const QStringList &fields);
    void handleErrorCmd(const QStringList &fields);

    QTcpSocket *m_socket;
    Player player;
    Login* loginWindow;
    QByteArray   readBuffer;


};

#endif // CLIENT_H
