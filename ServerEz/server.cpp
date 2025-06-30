
#include "server.h"
#include <QDebug>
#include <PreGame.h>

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

QStringList SERVER::extractFields(const QString &s) const
{
    QStringList list;
    int pos = 0;
    while (true) {
        int a = s.indexOf('[', pos);
        int b = s.indexOf(']', a + 1);
        if (a < 0 || b < 0) break;
        list << s.mid(a + 1, b - a - 1);
        pos = b + 1;
    }
    return list;
}

void SERVER::OnReadyRead()
{
    // 1) دریافت سوکت فرستنده
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client)  {
        qDebug() << "client is empty";
        return;
    }
    // 2) خواندن کل داده
    const QByteArray raw = client->readAll();
    qDebug() << "data : " << raw;
    if (raw.isEmpty())  {
        qDebug() << "Raw is empty";
        return;
    }
    // 3) جدا کردن دستور و payload
    const char cmd = raw.at(0);
    const QString payload = QString::fromUtf8(raw.mid(1));

    // 4) استخراج فیلدها
    const QStringList f = extractFields(payload);
    qDebug() << "type reciecved : " << cmd;


    // 5) فراخوانی متد مناسب
    switch (cmd) {
    case 'L': handleLogin(client, f);            break;
    case 'S': handleSignup(client, f);           break;
    case 'R': handleRecover(client, f);          break;
    case 'C': handleUpdateProfile(client, f);    break;
    case 'P': handlePreGame(client, f);          break;
    default:  handleUnknown(client);             break;
    }
}

//==============================================================================
// Login handler
//==============================================================================
// فرمت صحیح: L[type][id][pwd]
//  type ∈ {E,U}، id ایمیل یا یوزرنیم
// SERVER.cpp

void SERVER::handleLogin(QTcpSocket *client, const QStringList &f)
{
    // ۱) بررسی تعداد فیلدها
    if (f.size() != 3) {
        client->write("L[ERROR][BadFormat]");
        return;
    }

    const QString type = f[0].toUpper();
    const QString id   = f[1];
    const QString pwd  = f[2];

    // ۲) گرفتن اطلاعات عضو از دیتابیس
    QVariantMap member;
    if      (type == "E") member = db.GetMemberByEmail(id);
    else if (type == "U") member = db.GetMemberByUsername(id);
    else {
        client->write("L[ERROR][BadLoginType]");
        return;
    }

    // ۳) بررسی وجود عضو
    if (member.isEmpty()) {
        client->write("L[FAIL][Member not found]");
    }
    // ۴) بررسی صحت کلمه‌عبور
    else if (member.value("password").toString() != pwd) {
        client->write("L[WRONG][Wrong password]");
    }
    // ۵) همه‌چیز اوکی است ⇒ ارسال پاسخ OK با فیلدهای کامل
    else {
        // L[OK][successfull login][fn][ln][em][ph][un][pw]
        QString resp = QString(
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

        client->write(resp.toUtf8());
        clients[client] = member.value("username").toString();
    }
}


//==============================================================================
// Signup handler
//==============================================================================
// فرمت صحیح: S[first][last][email][phone][username][password]
void SERVER::handleSignup(QTcpSocket *client, const QStringList &f)
{
    if (f.size() != 6) {
        client->write("S[ERROR][BadFormat]");
        return;
    }
    const QString first = f[0], last  = f[1],
        email = f[2], phone = f[3],
        uname = f[4], pwd   = f[5];

    qDebug() << "Signup fields:" << first << last << email << phone << uname << pwd;

    // فرض: addMember خودش پیام مناسب را می‌فرستد
    bool ok = db.addMember(client, first, last, email, phone, uname, pwd);
    Q_UNUSED(ok);
}

//==============================================================================
// Recover handler
//==============================================================================
// فرمت: R[phone]
void SERVER::handleRecover(QTcpSocket *client, const QStringList &f)
{
    if (f.size() != 1) {
        client->write("R[ERROR][BadFormat]");
        return;
    }
    const QString phone = f[0];
    const QVariantMap member = db.GetMemberByPhone(phone);

    if (member.isEmpty()) {
        client->write("R[WRONG][Phone not registered]");
    }
    else {
        const QString resp = QString("R[OK][Password recovered][%1]")
        .arg(member.value("password").toString());
        client->write(resp.toUtf8());
    }
}

//==============================================================================
// Update Profile handler (C[CF]…)
//==============================================================================
// فرمت: C[CF][f1][…][f7]
void SERVER::handleUpdateProfile(QTcpSocket *client, const QStringList &f)
{
    if (f.isEmpty() || f[0] != "CF") {
        client->write("C[ERROR][BadFormat]");
        return;
    }
    if (f.size() != 8) {
        client->write("C[CF][ERROR][Bad format recieved]");
        qDebug() << "Bad format in C[CF]";
        return;
    }
    // f[1]…f[7]
    db.updateMemberAllFields(client,
                             f[1], f[2], f[3],
                             f[4], f[5], f[6], f[7]);
}

//==============================================================================
// Pre-game lobby handler
//==============================================================================
// فرمت: P[numPlayers]
void SERVER::handlePreGame(QTcpSocket *client, const QStringList &f)
{
    if (f.size() != 1) {
        client->write("P[ERROR][BadFormat]\n");
        return;
    }
    bool ok = false;
    int wantedPlayers = f[0].toInt(&ok);
    if (!ok || wantedPlayers < 2) {
        client->write("P[ERROR][BadMode]\n");
        return;
    }

    auto pair = qMakePair(client, clients.value(client));

    if (!lobby) {
        lobby = new PreGame(wantedPlayers, pair, this);
    }
    else if (!lobby->IsServerFull) {
        lobby->AddPlayer(pair);
    }
    else {
        QString resp = QString("P[%1][FULL][Server is full]\n")
        .arg(QString::number(wantedPlayers));
        client->write(resp.toUtf8());
    }
}



//==============================================================================
// Unknown command handler
//==============================================================================
void SERVER::handleUnknown(QTcpSocket *client)
{
    client->write("X[ERROR][UnknownCommand]");
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
