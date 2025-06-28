#include "client.h"
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDebug>
#include <player.h>
#include <qmessagebox.h>


Client::Client(QObject *parent)
    : QObject(parent),
    m_socket(new QTcpSocket(this))
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

    if(cmd == 'L') {

        if(fields[0] == "OK") {
            QMessageBox::information(nullptr,"Welcome!!","Successfully logined");
            player.SetInfo(fields[2],fields[3],fields[4],fields[5],fields[6],fields[7]);
            //we will add history data soon;
            return;

        }
        else {
            QMessageBox::information(nullptr,fields[0],fields[1]);
            return;
        }
    }
    else if(cmd == 'S') {

        if(fields.size() != 2) {
         qDebug() << "Bad format recieved for signup";
            return;
        }
        QMessageBox::information(nullptr,fields[0],fields[1]);
        if(fields[0] == "OK") {
            //تغییر پنجره به لاگین
        }
    }
    else if(cmd == 'R') {
        if(fields.size() != 3 && fields.size() != 2) {
            qDebug() << "Bad format recieved for signup";
            return;
        }
        if(fields[0] == "OK") {
            QMessageBox::information(nullptr,"Succesfull", fields[1] + "  : " + fields[2]);

        }
        else {
              QMessageBox::information(nullptr,fields[0], fields[1] );
        }
        return;

    }
    else if(cmd == 'X') {
         qDebug() << "Bad data type recieaved";
         QMessageBox::information(nullptr,fields[0], fields[1] );
    }

}
