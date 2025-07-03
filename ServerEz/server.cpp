#include "server.h"
#include <QDebug>
#include <QTimer>
#include "game.h"

SERVER::SERVER(MemberDatabaseManager* dbManager, const QString& address, QObject *parent)
    : QTcpServer(parent), dbManager(dbManager), address(address), port(8888), preGame(nullptr), game(nullptr)
{
    connect(this, &QTcpServer::newConnection, this, &SERVER::onNewConnection);
}

SERVER::~SERVER()
{
    if (preGame) {
        delete preGame;
    }
    if (game) {
        delete game;
    }
    for (auto client : clients.keys()) {
        client->close();
        delete client;
    }
}

void SERVER::setIP(const QString &address)
{
    this->address = address;
}

void SERVER::setPort(quint16 port)
{
    this->port = port;
}

void SERVER::onNewConnection()
{
    while (hasPendingConnections()) {
        QTcpSocket* client = nextPendingConnection();
        if (!client) {
            qDebug() << "Null client socket received";
            continue;
        }
        clients.insert(client, QString());
        qDebug() << "New client connected, total =" << clients.size();

        connect(client, &QTcpSocket::readyRead, this, &SERVER::onReadyRead);
        connect(client, &QTcpSocket::disconnected, this, &SERVER::onClientDisconnection);
    }
}

void SERVER::onReadyRead()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) {
        qDebug() << "Client is null in onReadyRead";
        return;
    }
    QByteArray raw = client->readAll();
    qDebug() << "Received raw data (full buffer):" << raw.toHex();

    while (!raw.isEmpty()) {
        int end = raw.indexOf('\n');
        if (end == -1) {
            qDebug() << "Incomplete message, waiting for more data:" << raw;
            break;
        }

        QByteArray line = raw.left(end);
        raw = raw.mid(end + 1);

        if (line.isEmpty()) {
            qDebug() << "Empty line skipped";
            continue;
        }

        QString message = QString::fromUtf8(line).trimmed();
        QChar cmd = message.at(0);
        QString payload = message.mid(1);
        QStringList fields = extractFields(payload);
        qDebug() << "Received from" << clients[client] << ": cmd =" << cmd << ", raw message =" << message << ", fields =" << fields;

        if (fields.isEmpty()) {
            handleUnknown(client);
            continue;
        }

        switch (cmd.unicode()) {
        case 'L': handleLogin(client, fields); break;
        case 'S': handleSignup(client, fields); break;
        case 'R': handleRecover(client, fields); break;
        case 'P': handlePreGame(client, fields); break;
        case 'C': handleUpdateProfile(client, fields); break;
        case 'G':
            if (game) {
                if (fields[0] == "CHAT") {
                    game->processChatMessage(client, fields);
                }
            } else {
                qDebug() << "Game object is null, cannot process chat";
            }
            break;
        default: handleUnknown(client); break;
        }
    }
}

void SERVER::handleLogin(QTcpSocket *client, const QStringList &fields)
{
    if (fields.size() != 3) {
        client->write("L[ERROR][Bad format: Expected L[type][id][password]]\n");
        client->flush();
        return;
    }

    QString type = fields[0];
    QString inputText = fields[1];
    QString password = fields[2];
    QString username;

    if (type == "U") {
        username = inputText;
    } else if (type == "E") {
        username = dbManager->getUsernameFromEmail(inputText);
        if (username.isEmpty()) {
            client->write("L[ERROR][Email not found]\n");
            client->flush();
            return;
        }
    } else {
        client->write("L[ERROR][Invalid login type: Must be U or E]\n");
        client->flush();
        return;
    }

    QVariantMap info = dbManager->getMemberInfo(username);
    if (info.isEmpty() || info["password"].toString() != password) {
        client->write("L[ERROR][Invalid username or password]\n");
        client->flush();
        return;
    }

    clients[client] = username;
    QString message = QString("L[OK][Successful login][%1][%2][%3][%4][%5][%6]")
                          .arg(info["firstName"].toString(),
                               info["lastName"].toString(),
                               info["phone"].toString(),
                               info["email"].toString(),
                               username,
                               info["password"].toString());
    QList<QVariantMap> history = dbManager->getGameHistory(username);
    if (!history.isEmpty()) {
        message += "[HIST]";
        for (const auto &game : history) {
            message += QString("[%1,%2,%3]")
            .arg(game["player1_username"].toString(),
                 game["player2_username"].toString(),
                 game["winner_username"].toString());
        }
    }
    message += "\n";
    client->write(message.toUtf8());
    client->flush();
}

void SERVER::onClientDisconnection()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    QString username = clients.value(client);
    qDebug() << "Client" << username << "disconnected";
    clients.remove(client);
    client->deleteLater();

    if (preGame && !preGame->isFull()) {
        delete preGame;
        preGame = nullptr;
        qDebug() << "PreGame cancelled due to disconnection";
    }
    if (game) {
        delete game;
        game = nullptr;
        qDebug() << "Game deleted due to disconnection";
    }
}

void SERVER::onStartGame(const QStringList &players)
{
    qDebug() << "Starting game with players:" << players;
    if (game) {
        delete game;
    }
    game = new Game(players, clients, *dbManager, this);
    connect(game, &Game::gameFinished, this, [this]() {
        if (game) {
            delete game;
            game = nullptr;
        }
        preGame = nullptr;
        qDebug() << "Game finished, lobby reset";
    });
    game->start();
}

void SERVER::handleSignup(QTcpSocket *client, const QStringList &fields)
{
    if (fields.size() != 6) {
        client->write("S[ERROR][Bad format: Expected S[firstname][lastname][phone][email][username][password]]\n");
        client->flush();
        return;
    }

    QString firstName = fields[0];
    QString lastName = fields[1];
    QString phone = fields[2];
    QString email = fields[3];
    QString username = fields[4];
    QString password = fields[5];

    if (!dbManager->isValidSignup(firstName, lastName, phone, email, username, password)) {
        client->write("S[ERROR][All fields must be non-empty]\n");
        client->flush();
        return;
    }

    if (dbManager->addMember(firstName, lastName, phone, email, username, password)) {
        client->write("S[OK][Signup successful]\n");
        client->flush();
    } else {
        client->write("S[ERROR][Username or email already exists]\n");
        client->flush();
    }
}

void SERVER::handleRecover(QTcpSocket *client, const QStringList &fields)
{
    if (fields.size() != 1) {
        client->write("R[ERROR][Bad format: Expected R[phone]]\n");
        client->flush();
        return;
    }

    QString phone = fields[0];
    QString password = dbManager->getPasswordFromPhone(phone);
    if (password.isEmpty()) {
        client->write("R[ERROR][Phone number not found]\n");
        client->flush();
    } else {
        client->write(QString("R[OK][Password found][%1]\n").arg(password).toUtf8());
        client->flush();
    }
}

void SERVER::handlePreGame(QTcpSocket *client, const QStringList &fields)
{
    if (fields.size() != 1) {
        client->write("P[ERROR][Bad format: Expected P[mode]]\n");
        client->flush();
        return;
    }

    bool ok;
    int mode = fields[0].toInt(&ok);
    if (!ok || mode != 2) {
        client->write("P[ERROR][Only 2-player mode supported]\n");
        client->flush();
        return;
    }

    QString username = clients[client];
    if (username.isEmpty()) {
        client->write("P[ERROR][Not logged in]\n");
        client->flush();
        return;
    }

    if (!preGame) {
        preGame = new PreGame(2, {client, username}, this);
        connect(preGame, &PreGame::startGame, this, &SERVER::onStartGame);
    } else if (!preGame->isFull()) {
        preGame->addPlayer({client, username});
    } else {
        client->write("P[FULL][Server is full]\n");
        client->flush();
    }
}

void SERVER::handleUpdateProfile(QTcpSocket *client, const QStringList &fields)
{
    if (fields.size() != 8 || fields[0] != "CF") {
        client->write("C[ERROR][Bad format: Expected C[CF][firstname][lastname][phone][email][newUsername][password][oldUsername]]\n");
        client->flush();
        return;
    }

    QString username = clients[client];
    if (username.isEmpty()) {
        client->write("C[ERROR][Not logged in]\n");
        client->flush();
        return;
    }

    QString firstName = fields[1];
    QString lastName = fields[2];
    QString phone = fields[3];
    QString email = fields[4];
    QString newUsername = fields[5];
    QString password = fields[6];
    QString oldUsername = fields[7];

    if (oldUsername != username) {
        client->write("C[ERROR][Old username does not match logged-in user]\n");
        client->flush();
        return;
    }

    if (dbManager->updateMemberAllFields(oldUsername, firstName, lastName, phone, email, newUsername, password)) {
        clients[client] = newUsername;
        client->write(QString("C[CF][OK][Information updated successfully][%1][%2][%3][%4][%5][%6]\n")
                          .arg(firstName, lastName, phone, email, newUsername, password).toUtf8());
        client->flush();
    } else {
        client->write("C[CF][ERROR][Update failed: Username or email may already exist]\n");
        client->flush();
    }
}

void SERVER::handleUnknown(QTcpSocket *client)
{
    client->write("X[ERROR][Unknown command]\n");
    client->flush();
}

QStringList SERVER::extractFields(const QString &payload) const
{
    QStringList fields;
    int pos = 0;
    while (true) {
        int start = payload.indexOf('[', pos);
        int end = payload.indexOf(']', start + 1);
        if (start < 0 || end < 0) break;
        fields << payload.mid(start + 1, end - start - 1);
        pos = end + 1;
    }
    return fields;
}
