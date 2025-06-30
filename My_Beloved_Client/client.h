#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QHostAddress>
#include "player.h"

class QTcpSocket;
class Login;
class Connect;
class Signup;
class Choose_mode;

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


    void SetConnect(Connect *connect) { m_connect = connect; }
    void SetSignup(Signup *signup) { m_signup = signup; }
    void SetChooseMode(Choose_mode *chooseMode) { m_chooseMode = chooseMode; }

    void handleUpdateProfile(const QStringList &fields);
private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);
    void OnReadyRead();
    void handlePreGame(const QStringList &f);

private:
    QStringList extractFields(const QString &msg);
    void handleLogin(const QStringList &fields);
    void handleSignup(const QStringList &fields);
    void handleRecover(const QStringList &fields);
    void handleErrorCmd(const QStringList &fields);

    QTcpSocket *m_socket;
    Player player;
    Login* loginWindow;
    QByteArray readBuffer;
    Connect *m_connect;
    Signup *m_signup;
    Choose_mode *m_chooseMode;
};

#endif // CLIENT_H
