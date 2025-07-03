#include "manager.h"
#include <QNetworkInterface>
#include <QDebug>

MANAGER::MANAGER(QObject *parent) : QObject(parent)
{
    QString baseDir = "C:/Users/HoSaiN/Desktop/HoSaiN_Apps_Edits/HoSaiN_Qt/Card game/ServerEz";
    QString dbPath = baseDir + "/members.sqlite";

    db = new MemberDatabaseManager(dbPath, this);
    QString address = getLocalIPAddress();
    server = new SERVER(db, address, this);
    server->setPort(8888);

    if (!server->listen(QHostAddress(address), 8888)) {
        qDebug() << "Server could not start on" << address << ":" << 8888
                 << "— error:" << server->errorString();
    } else {
        qDebug() << "Server started on" << address << ":" << 8888;
    }
}

MANAGER::~MANAGER()
{
    delete server;
    delete db;
}

QString MANAGER::getLocalIPAddress()
{
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    for (const QHostAddress &addr : addresses) {
        if (addr.protocol() == QAbstractSocket::IPv4Protocol && !addr.isLoopback()) {
            return addr.toString();
        }
    }
    qWarning() << "No valid IPv4 address found, falling back to QHostAddress::Any";
    return QHostAddress(QHostAddress::Any).toString();
}
