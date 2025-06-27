#include "client.h"
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDebug>

Client::Client(QObject *parent)
    : QObject(parent),
    m_socket(new QTcpSocket(this))
{
    connect(m_socket, &QTcpSocket::connected,
            this,     &Client::onConnected);
    connect(m_socket, &QTcpSocket::disconnected,
            this,     &Client::onDisconnected);
    connect(m_socket,
            QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, &Client::onError);
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
