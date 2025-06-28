#include "client.h"
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDebug>
#include <player.h>
#include <qmessagebox.h>
#include <login.h>


Client::Client(QObject *parent)
    : QObject(parent),
    m_socket(new QTcpSocket(this))
    ,loginWindow( new Login(player,this))

{
    connect(m_socket, &QTcpSocket::connected,
            this,     &Client::onConnected);
    connect(m_socket, &QTcpSocket::disconnected,
            this,     &Client::onDisconnected);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this,     &Client::onError);
    connect(m_socket, &QTcpSocket::readyRead,
            this,     &Client::OnReadyRead);
}

Client::~Client()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        m_socket->disconnectFromHost();
    // مفسر Qt‌ خودش m_socket را خواهد کُشت چون parent آن این است.
}

void Client::ConnectToServer(const QHostAddress &host, quint16 port)
{
    m_socket->connectToHost(host, port);
}

void Client::DisconnectFromServer()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        m_socket->disconnectFromHost();
}

void Client::WriteToServer(const QString &data)
{
    m_socket->write(data.toUtf8());
}

void Client::onConnected()
{
    qDebug() << "Client::onConnected";
    emit ConnectedToServer();
}

void Client::onDisconnected()
{
    qDebug() << "Client::onDisconnected";
    emit DisconnectedFromServer();
}

void Client::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    QString err = m_socket->errorString();
    qDebug() << "Client::onError:" << err;
    emit ErrorOccurred(err);
}

void Client::OnReadyRead()
{
    QByteArray raw = m_socket->readAll();
    if (raw.isEmpty())
        return;

    char cmd = raw.at(0);
    QString payload = QString::fromUtf8(raw.mid(1));

    auto extractFields = [&](const QString& s)->QStringList {
        QStringList fields;
        int pos = 0;
        while (true) {
            int a = s.indexOf('[', pos);
            int b = s.indexOf(']', a+1);
            if (a < 0 || b < 0) break;
            fields << s.mid(a+1, b-a-1);
            pos = b + 1;
        }
        return fields;
    };

    QStringList fields = extractFields(payload);

    if (cmd == 'L') { // LOGIN
        if (fields.isEmpty()) return;
        if (fields[0] == "OK") {
            // fields: [OK][msg][fn][ln][em][ph][un][pw]
            if (fields.size() < 8) {
                QMessageBox::warning(nullptr, "Login", "Login response bad format!");
                return;
            }
            QMessageBox::information(nullptr, "Welcome!", "Successfully logined");
            player.SetInfo(fields[2], fields[3], fields[4], fields[5], fields[6], fields[7]);
            emit SuccesFull_LogIn();
        } else {
            if (fields.size() < 2) return;
            QMessageBox::warning(nullptr, fields[0], fields[1]);
        }
    }
    else if (cmd == 'S') { // SIGNUP
        if (fields.size() < 2) return;
        if (fields[0] == "OK") {
            QMessageBox::information(nullptr, "Signup", fields[1]);
            emit SuccesFullSignUp();
            loginWindow->show();

        } else {
            QMessageBox::warning(nullptr, fields[0], fields[1]);
        }
    }
    else if (cmd == 'R') { // RECOVER
        if (fields.isEmpty()) return;
        if (fields[0] == "OK") {
            if (fields.size() < 3) return;
            QMessageBox::information(nullptr, "Password found", QString("%1 : %2").arg(fields[1], fields[2]));
        } else {
            if (fields.size() < 2) return;
            QMessageBox::warning(nullptr, fields[0], fields[1]);
        }
    }
    else if (cmd == 'X') {
        if (fields.size() < 2) return;
        QMessageBox::warning(nullptr, fields[0], fields[1]);
    }
}

Player& Client::GetPlayer() {
    return player;
}

