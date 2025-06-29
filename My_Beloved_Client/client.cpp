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

void Client::WriteToServer(const QString &msg)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        m_socket->write(msg.toUtf8());
}

void Client::onConnected()    { qDebug() << "Connected"; emit ConnectedToServer(); }
void Client::onDisconnected() { qDebug() << "Disconnected"; emit DisconnectedFromServer(); }
void Client::onError(QAbstractSocket::SocketError) {
    QString err = m_socket->errorString();
    qDebug() << "Error:" << err;
    emit ErrorOccurred(err);
}

QStringList Client::extractFields(const QString &msg) const
{
    QStringList out;
    int pos = 0;
    while (true) {
        int b = msg.indexOf('[', pos);
        if (b < 0) break;
        int e = msg.indexOf(']', b);
        if (e < 0) break;
        out << msg.mid(b+1, e-b-1);
        pos = e + 1;
    }
    return out;
}

void Client::OnReadyRead()
{
    // 1) Read all available bytes once
    readBuffer.append(m_socket->readAll());

    // 2) While there's at least one '\n' in our buffer, extract complete lines
    int idx = -1;
    while ((idx = readBuffer.indexOf('\n')) != -1) {
        // Extract up to (but not including) '\n'
        QByteArray line = readBuffer.left(idx);
        // Remove that line + the '\n' from the buffer
        readBuffer.remove(0, idx + 1);

        if (line.isEmpty())
            continue;

        // 3) Parse command + payload from this single line
        char cmd = line.at(0);                      // first byte = command
        QString payload = QString::fromUtf8(line.mid(1));
        QStringList fields = extractFields(payload);
        if (fields.isEmpty())
            continue;

        // 4) Dispatch based on command
        switch (cmd) {
            case 'L': handleLogin(fields);      break;
            case 'S': handleSignup(fields);     break;
            case 'R': handleRecover(fields);    break;
            case 'P': handlePreGame(fields);    break;
            case 'X': handleErrorCmd(fields);   break;
            default:
                // unknown command – you might log or ignore
                break;
        }
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
    emit SuccesFull_LogIn();
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
