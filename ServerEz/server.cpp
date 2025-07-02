#include "server.h"
#include <QDebug>
#include "pregame.h"
#include "game.h"

SERVER::SERVER(MemberDatabaseManager& db, const QString& address, int port, QObject* parent)
    : QObject(parent), server(new QTcpServer(this)), total(0), db(db), lobby(nullptr), game(nullptr)
{
    if (!db.open()) {
        qWarning() << "Cannot open member database!";
        return;
    }
    db.createMemberTable();

    if (!server->listen(QHostAddress(address), port)) {
        qWarning() << "Server could not start on" << address << ":" << port
                   << "— error:" << server->errorString();
        return;
    }

    qDebug() << "Server started on" << address << ":" << port;
    connect(server, &QTcpServer::newConnection, this, &SERVER::OnNewConnection);
}

SERVER::~SERVER()
{
    if (lobby) delete lobby;
    if (game) delete game;
    for (auto client : clients.keys()) {
        client->deleteLater();
    }
    server->close();
}

void SERVER::OnNewConnection()
{
    while (server->hasPendingConnections()) {
        QTcpSocket* client = server->nextPendingConnection();
        if (!client) {
            qDebug() << "Null client socket received";
            continue;
        }
        clients.insert(client, QStringLiteral(""));
        ++total;
        qDebug() << "New client connected, total =" << total;

        connect(client, &QTcpSocket::readyRead, this, &SERVER::OnReadyRead);
        connect(client, &QTcpSocket::disconnected, this, [this, client]() {
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
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) {
        qDebug() << "Client is null in OnReadyRead";
        return;
    }
    QByteArray raw = client->readAll();
    if (raw.isEmpty()) {
        qDebug() << "Raw data is empty";
        return;
    }

    QString message = QString::fromUtf8(raw).trimmed();
    QStringList parts = message.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : parts) {
        char cmd = line.at(0);
        QString payload = line.mid(1);
        QStringList fields = extractFields(payload);
        qDebug() << "Type received:" << cmd;

        switch (cmd) {
        case 'L': handleLogin(client, fields); break;
        case 'S': handleSignup(client, fields); break;
        case 'R': handleRecover(client, fields); break;
        case 'C': handleClientCommand(client, fields); break;
        case 'P': handlePreGame(client, fields); break;
        case 'G':
            if (game) game->handleClientMessage(client, fields);
            else handleUnknown(client);
            break;
        default: handleUnknown(client); break;
        }
    }
}
void SERVER::onGameStarted(const QStringList& players)
{
    qDebug() << "Game started with players:" << players;
    if (lobby) {
        delete lobby;
        lobby = nullptr;
    }
    game = new Game(players, clients, this);
    connect(game, &Game::gameFinished, this, &SERVER::onGameFinished);
    game->start();
}

void SERVER::onGameFinished()
{
    if (game) {
        delete game;
        game = nullptr;
    }
}

void SERVER::handleLogin(QTcpSocket *client, const QStringList &f)
{
    if (f.size() != 3) {
        client->write("L[ERROR][Bad format received]\n");
        client->flush();
        return;
    }

    const QString type = f[0].toUpper();
    const QString id = f[1];
    const QString pwd = f[2];

    QVariantMap member;
    if (type == "E") member = db.GetMemberByEmail(id);
    else if (type == "U") member = db.GetMemberByUsername(id);
    else {
        client->write("L[ERROR][Invalid login type]\n");
        client->flush();
        return;
    }

    if (member.isEmpty()) {
        client->write("L[FAIL][Member not found]\n");
        client->flush();
        return;
    }

    if (member.value("password").toString() != pwd) {
        client->write("L[WRONG][Wrong password]\n");
        client->flush();
        return;
    }

    QString resp = QString(
                       "L[OK][Successful login][%1][%2][%3][%4][%5][%6]\n"
                       ).arg(
                           member.value("firstname").toString(),
                           member.value("lastname").toString(),
                           member.value("email").toString(),
                           member.value("phone").toString(),
                           member.value("username").toString(),
                           member.value("password").toString()
                           );

    client->write(resp.toUtf8());
    client->flush();
    clients[client] = member.value("username").toString();
}

void SERVER::handleSignup(QTcpSocket *client, const QStringList &f)
{
    if (f.size() != 6) {
        client->write("S[ERROR][Bad format received]\n");
        client->flush();
        return;
    }
    const QString first = f[0], last = f[1],
        email = f[2], phone = f[3],
        uname = f[4], pwd = f[5];

    db.addMember(client, first, last, email, phone, uname, pwd);
}

void SERVER::handleRecover(QTcpSocket *client, const QStringList &f)
{
    if (f.size() != 1) {
        client->write("R[ERROR][Bad format received]\n");
        client->flush();
        return;
    }
    const QString phone = f[0];
    const QVariantMap member = db.GetMemberByPhone(phone);

    if (member.isEmpty()) {
        client->write("R[WRONG][Phone not registered]\n");
        client->flush();
    } else {
        const QString resp = QString("R[OK][Password recovered][%1]\n")
        .arg(member.value("password").toString());
        client->write(resp.toUtf8());
        client->flush();
    }
}

void SERVER::handleClientCommand(QTcpSocket *client, const QStringList &f)
{
    if (f.size() < 1) {
        client->write("C[ERROR][Invalid command format]\n");
        client->flush();
        return;
    }

    if (f[0].toUpper() == "CF") {
        if (f.size() != 8) {
            client->write("C[CF][ERROR][Invalid number of fields]\n");
            client->flush();
            return;
        }

        db.updateMemberAllFields(client, f[7], f[1], f[2], f[3], f[4], f[5], f[6]);
    } else {
        client->write("C[ERROR][Unknown client command]\n");
        client->flush();
    }
}

void SERVER::handlePreGame(QTcpSocket *client, const QStringList &f)
{
    if (f.size() != 1) {
        client->write("P[ERROR][Bad format received]\n");
        client->flush();
        return;
    }
    bool ok = false;
    int wantedPlayers = f[0].toInt(&ok);
    if (!ok || wantedPlayers < 2 || wantedPlayers > 4) { // محدود کردن تعداد بازیکنان
        client->write("P[ERROR][Invalid mode]\n");
        client->flush();
        return;
    }

    auto pair = qMakePair(client, clients.value(client));
    if (pair.second.isEmpty()) {
        client->write("P[ERROR][You must login first]\n");
        client->flush();
        return;
    }

    if (!lobby || lobby->IsServerFull) {
        lobby = new PreGame(wantedPlayers, pair, this);
        connect(lobby, &PreGame::startGame, this, &SERVER::onGameStarted);
    } else {
        lobby->AddPlayer(pair);
    }
}

void SERVER::handleUnknown(QTcpSocket *client)
{
    client->write("X[ERROR][Unknown command]");
    client->flush();
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

void SERVER::onGameStarted(const QStringList& players)
{
    qDebug() << "Game started with players:" << players;
    // در اینجا می‌توانید منطق شروع بازی را اضافه کنید
    if (lobby) {
        delete lobby;
        lobby = nullptr;
    }
}
