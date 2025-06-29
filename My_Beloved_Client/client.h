// Client.h
#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QStringList>

class Login;

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    void connectToServer(const QString &host, quint16 port);
    void writeToServer(const QByteArray &data);
    void setLoginWindow(Login *loginWindow);

signals:
    void successfulLogin(const QString &firstName, const QString &lastName);
    void signupSuccessful();
    void recoverSuccessful();

private slots:
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);

private:
    QStringList extractFields(const QString &msg);
    void handleLogin(const QStringList &fields);
    void handleSignup(const QStringList &fields);
    void handleRecover(const QStringList &fields);
    void handlePreGame(const QStringList &fields);
    void handleErrorCmd(const QStringList &fields);

    QTcpSocket *socket;
    QByteArray readBuffer;
    Login *loginWindow;
};

#endif // CLIENT_H
