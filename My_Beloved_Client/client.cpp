#include "client.h"
#include <QDebug>
#include <QAbstractSocket>

Client::Client(QObject* parent)
    : QObject(parent),
    socket(new QTcpSocket(this))
{
    connect(socket, &QTcpSocket::connected,
            this,   &Client::onConnected);
    connect(socket, &QTcpSocket::errorOccurred,
            this,   &Client::onError);
}

void Client::ConnectToServer(const QHostAddress& address, int port)
{
    qDebug() << "Attempting to connect to" << address.toString() << ":" << port;
    socket->connectToHost(address, port);

}

void Client::onConnected()
{
    qDebug() << "Socket connected!";
    emit ConnectedToServer();
}

void Client::onError(QAbstractSocket::SocketError /*socketError*/)
{
    qWarning() << "Socket error:" << socket->errorString();

}
