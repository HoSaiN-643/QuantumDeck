
#include "server.h"
#include <QDebug>

SERVER::SERVER(MemberDatabaseManager& db,
               const QString& address,
               int port,
               QObject* parent)
    : QObject(parent)
    , server(new QTcpServer(this))
    , total(0)
    , db(db)
{

    if (!db.open()) {
        qWarning() << "Cannot open member database!";
        return;
    }
    db.createMemberTable();


    if (!server->listen(QHostAddress(address), port)) {
        qWarning() << "Server could not start on"
                   << address << ":" << port
                   << "— error:" << server->errorString();
        return;
    }

    qDebug() << "Server started on" << address << ":" << port;
    connect(server, &QTcpServer::newConnection,
            this,   &SERVER::OnNewConnection);
}

void SERVER::OnNewConnection()
{
    while (server->hasPendingConnections()) {
        QTcpSocket* client = server->nextPendingConnection();
        clients.insert(client, QStringLiteral(""));
        ++total;
        qDebug() << "New client connected, total =" << total;


        connect(client, &QTcpSocket::readyRead,
                this,   &SERVER::OnReadyRead);


        connect(client, &QTcpSocket::disconnected,
                this, [this,client](){
                    OnClientDisconnection(client);
                });
    }
}

void SERVER::OnReadyRead()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (!client) {
        qWarning() << "OnReadyRead: sender is not a QTcpSocket";
        return;
    }

    QByteArray raw = client->readAll();
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


    if (cmd == 'L') {

        QStringList f = extractFields(payload);
        if (f.size() != 3) {
            client->write("L[ERROR][BadFormat]");
            qWarning() << "Login: bad format:" << payload;
            return;
        }
        QString type = f.at(0).toUpper();
        QString id   = f.at(1);
        QString pwd  = f.at(2);
        QString uname;
        QVariantMap member;
        if (type == QStringLiteral("E")) {
            member = db.GetMemberByEmail(id);
            uname = member.value("username").toString();

            qDebug() << "Login with Email:" << id;
        }
        else if (type == QStringLiteral("U")) {
            member = db.GetMemberByUsername(id);
            qDebug() << "Login with Username:" << id;
            uname = member.value("username").toString();

        }
        else {
            client->write("L[ERROR][BadLoginType]");
            qWarning() << "Login: bad login type:" << type;
            return;
        }

        if (member.isEmpty()) {
            client->write("L[FAIL][Member not found]");
            qDebug() << "Member not found for" << id;
        }
        else if (member.value("password") != pwd) {
            client->write("L[WRONG][Wrong password]");
            qDebug() << "Wrong password for" << id;
        }
        else {
            client->write("L[OK][Login successful]");
            clients[client] = uname;
            qDebug() << "Login successful for" << id;
        }
    }
    else if (cmd == 'S') {

        QStringList f = extractFields(payload);
        if (f.size() != 6) {
            client->write("S[ERROR][BadFormat]");
            qWarning() << "Signup: bad format:" << payload;
            return;
        }
        QString FirstName   = f.at(0);
        QString LastName    = f.at(1);
        QString PhoneNumber = f.at(2);
        QString uname       = f.at(3);
        QString email       = f.at(4);
        QString pwd_        = f.at(5);


        db.addMember(client, uname, email, pwd_, FirstName, LastName, PhoneNumber);
        qDebug() << "Signup request for" << uname;
    }
    else if (cmd == 'R') {

        QStringList f = extractFields(payload);
        if (f.size() != 1) {
            client->write("R[ERROR][BadFormat]");
            qWarning() << "Recover: bad format:" << payload;
            return;
        }
        QString phone = f.at(0);
        QVariantMap member = db.GetMemberByPhone(phone);
        if (member.isEmpty()) {
            client->write("R[WRONG][Phone not registered]");
            qDebug() << "Recover: phone not found:" << phone;
        } else {
            QByteArray resp = "R[OK][Password recovered]["
                              + member.value("password").toByteArray()
                              + "]";
            client->write(resp);
            qDebug() << "Recover: password sent for" << phone;
        }
    }
    else {
        client->write("X[ERROR][UnknownCommand]");
        qWarning() << "Unknown command:" << cmd << "payload:" << payload;
    }
}

void SERVER::OnClientDisconnection(QTcpSocket* client)
{
    QString name = clients.value(client);
    qDebug() << "Client" << name << "disconnected";
    clients.remove(client);
    --total;
    qDebug() << "Total clients now =" << total;
    client->deleteLater();
}
