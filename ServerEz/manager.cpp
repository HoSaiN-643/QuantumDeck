#include "manager.h"
#include <server.h>
#include <memberdatabasemanager.h>
#include <QTcpServer>
#include <QCoreApplication>
#include <QNetworkInterface>
#include <QDebug>

MANAGER::MANAGER() {
    QString baseDir = "C:/Users/HoSaiN/Desktop/HoSaiN_Apps_Edits/HoSaiN_Qt/Card game/ServerEz";
    QString dbPath = baseDir + "/members.sqlite";

    // Initialize the database
    db = new MemberDatabaseManager(dbPath);

    // Get the system's IP address dynamically
    QString address = getLocalIPAddress();
    qDebug() << "Using IP address:" << address;

    // Initialize the server with the dynamic IP address
    server = new SERVER(*db, address);
}

QString MANAGER::getLocalIPAddress() {
    // Get all network interfaces
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();

    // Look for a non-loopback IPv4 address
    for (const QHostAddress &addr : addresses) {
        if (addr.protocol() == QAbstractSocket::IPv4Protocol && !addr.isLoopback()) {
            return addr.toString();
        }
    }

    // Fallback: If no suitable IP is found, use QHostAddress::Any (0.0.0.0)
    qWarning() << "No valid IPv4 address found, falling back to QHostAddress::Any";
    return QHostAddress(QHostAddress::Any).toString();
}
