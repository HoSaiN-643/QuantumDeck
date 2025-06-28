
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
    if (!client) return;

    QByteArray raw = client->readAll();
    if (raw.isEmpty()) return;

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

    if (cmd == 'L') { // LOGIN
        QStringList f = extractFields(payload);
        if (f.size() != 3) {
            client->write("L[ERROR][BadFormat]");
            return;
        }
        QString type = f[0].toUpper();
        QString id   = f[1];
        QString pwd  = f[2];
        QVariantMap member;
        if (type == "E")
            member = db.GetMemberByEmail(id);
        else if (type == "U")
            member = db.GetMemberByUsername(id);
        else {
            client->write("L[ERROR][BadLoginType]");
            return;
        }
        if (member.isEmpty()) {
            client->write("L[FAIL][Member not found]");
        }
        else if (member.value("password").toString() != pwd) {
            client->write("L[WRONG][Wrong password]");
        }
        else {
            // همه فیلدها را بفرست
            QString data = QString(
                               "L[OK][successfull login]"
                               "[%1][%2][%3][%4][%5][%6]"
                               ).arg(
                                   member.value("firstname").toString(),
                                   member.value("lastname").toString(),
                                   member.value("email").toString(),
                                   member.value("phone").toString(),
                                   member.value("username").toString(),
                                   member.value("password").toString()
                                   );
            client->write(data.toUtf8());
            clients[client] = member.value("username").toString();
        }
    }
    else if (cmd == 'S') { // SIGNUP
        QStringList f = extractFields(payload);
        if (f.size() != 6) {
            client->write("S[ERROR][BadFormat]");
            return;
        }
        // ترتیب دقیق: [first][last][email][phone][username][password]
        const QString first  = f.at(0);
        const QString last   = f.at(1);
        const QString email  = f.at(2);
        const QString phone  = f.at(3);
        const QString uname  = f.at(4);
        const QString pwd    = f.at(5);

        // لاگ برای اطمینان
        qDebug() << "Signup fields:"
                 << first << last << email << phone << uname << pwd;

        // فراخوانی با ترتیب جدید
        bool ok = db.addMember(client,
                               first,    // firstname
                               last,     // lastname
                               email,    // email
                               phone,    // phone
                               uname,    // username
                               pwd);     // password

        // (می‌توانید درون addMember پیام ارسال می‌شود)
    }
    else if (cmd == 'R') { // RECOVER
        QStringList f = extractFields(payload);
        if (f.size() != 1) {
            client->write("R[ERROR][BadFormat]");
            return;
        }
        QString phone = f[0];
        QVariantMap member = db.GetMemberByPhone(phone);
        if (member.isEmpty()) {
            client->write("R[WRONG][Phone not registered]");
        } else {
            QString resp = QString("R[OK][Password recovered][%1]").arg(member.value("password").toString());
            client->write(resp.toUtf8());
        }
    }
    else if(cmd == 'C') {
       QStringList f = extractFields(payload);
        if(f[0] == "CF") { // updating profile;
           if(f.size() != 8) {
               client->write("C[CF][ERROR][Bad format recieved]");
               qDebug() << "Bad format Recieved";
           }
           db.updateMemberAllFields(client,f[1],f[2],f[3],f[4],f[5],f[6],f[7]);

        }

    }
    else {
        client->write("X[ERROR][UnknownCommand]");
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
