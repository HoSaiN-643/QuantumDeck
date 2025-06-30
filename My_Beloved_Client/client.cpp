#include "client.h"
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDebug>
#include <QMessageBox>
#include "login.h"
#include "player.h"

Client::Client(QObject *parent)
    : QObject(parent),
    m_socket(new QTcpSocket(this)),
    loginWindow(new Login(player, this))
{
    connect(m_socket, &QTcpSocket::connected,    this, &Client::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(m_socket,
            QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, &Client::onError);
    connect(m_socket, &QTcpSocket::readyRead,    this, &Client::OnReadyRead);
}

Client::~Client()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        m_socket->disconnectFromHost();
}

void Client::ConnectToServer(const QHostAddress &host, quint16 port)
{ m_socket->connectToHost(host, port); }

void Client::DisconnectFromServer()
{ if (m_socket->state() == QAbstractSocket::ConnectedState)
        m_socket->disconnectFromHost(); }



void Client::onConnected()    { qDebug() << "Connected"; emit ConnectedToServer(); }
void Client::onDisconnected() { qDebug() << "Disconnected"; emit DisconnectedFromServer(); }
void Client::onError(QAbstractSocket::SocketError) {
    QString err = m_socket->errorString();
    qDebug() << "Error:" << err;
    emit ErrorOccurred(err);
}

QStringList Client::extractFields(const QString &msg)
{
    QStringList result;
    int start = msg.indexOf('[');
    while (start != -1) {
        int end = msg.indexOf(']', start);
        if (end == -1) break;
        result << msg.mid(start + 1, end - start - 1);
        start = msg.indexOf('[', end);
    }
    return result;
}

void Client::OnReadyRead()
{
    // 1) کل داده رو یکجا بخون (نه append)
    readBuffer = m_socket->readAll();


    QByteArray line =readBuffer;


    readBuffer.clear();

    // 4) اگر خالیه ولش کن
    if (line.isEmpty())
        return;

    // 5) پردازش مثل قبل
    char cmd        = line.at(0);
    QByteArray body = line.mid(1);
    QString payload = QString::fromUtf8(body);
    QStringList fields = extractFields(payload);
    if (fields.isEmpty())
        return;

    switch (cmd) {
    case 'L': handleLogin(fields);    break;
    case 'S': handleSignup(fields);   break;
    case 'R': handleRecover(fields);  break;
    case 'P': handlePreGame(fields);  break;
    case 'X': handleErrorCmd(fields); break;
    default:  break;
    }
}


void Client::handleLogin(const QStringList &fields) {
    // fields: [OK|ERR][msg][fn][ln][em][ph][un][pw]
    // fields: [OK|ERR][msg][fn][ln][em][ph][un][pw]
    if (fields.isEmpty() || fields[0] != "OK") {
        if (fields.size() >= 2)
            QMessageBox::information(loginWindow,
                                     tr("Login failed"),
                                     fields[1]);
        return;
    }

    if (fields.size() < 8) {
        QMessageBox::information(loginWindow,
                                 tr("Login"),
                                 tr("Login response bad format!"));
        return;
    }

    QMessageBox::information(loginWindow,
                             tr("Welcome!"),
                             tr("Successfully logged in"));
    player.SetInfo(fields[2], fields[3],
                   fields[4], fields[5],
                   fields[6], fields[7]);
    loginWindow->Open_menu();
}

void Client::handleSignup(const QStringList &fields) {
    // fields: [OK|ERR][msg]
    if (fields[0] == "OK") {
        if (fields.size() >= 2)
            QMessageBox::information(loginWindow,
                                     tr("Signup"),
                                     fields[1]);
        emit SuccesFullSignUp();
        loginWindow->show();
    } else {
        if (fields.size() >= 2)
            QMessageBox::information(loginWindow,
                                     tr("Signup failed"),
                                     fields[1]);
    }
}

void Client::handleRecover(const QStringList &fields) {
    // fields: [OK|ERR][msg][password?]
    if (fields[0] == "OK") {
        if (fields.size() >= 3)
            QMessageBox::information(loginWindow,
                                     tr("Password found"),
                                     QString("%1 : %2").arg(fields[1], fields[2]));
    } else {
        if (fields.size() >= 2)
            QMessageBox::information(loginWindow,
                                     tr("Recover failed"),
                                     fields[1]);
    }
}

void Client::handleErrorCmd(const QStringList &fields) {
    // fields: [ERR][msg]
    if (fields.size() >= 2)
        QMessageBox::information(loginWindow,
                                 tr("Error"),
                                 fields[1]);
}


Player& Client::GetPlayer() { return player; }

void Client::WriteToServer(const QString &data)
{
    QByteArray out = data.toUtf8();

    m_socket->write(out);
}

void Client::handlePreGame(const QStringList &f) {
    // f[0] = total، f[1] = وضعیت، f[2...] = آرگومان‌ها
    if (f.size() < 2) return;

    int total = f[0].toInt();
    const QString &status = f[1];

    // حالت جست‌وجو
    if (f.size() >= 3 && f[2] == QLatin1String("Searching for a match")) {
        int waiting = total;        // دقت کنید: در پیامِ Searching فیلد second شمارۀ waiting است
        waiting = f[1].toInt();
        emit PreGameSearching(waiting, total);
    }
    // حالت پیدا شد
    else if (status == QLatin1String("Found")) {
        // f = [ total, "Found", name1, name2, ... ]
        QStringList others = f.mid(2);
        emit PreGameFound(others);
    }
    // حالت فول
    else if (status == QLatin1String("FULL")) {
        QString msg = (f.size() >= 3 ? f[2]
                                     : tr("Server is full"));
        emit PreGameFull(msg);
    }
}
