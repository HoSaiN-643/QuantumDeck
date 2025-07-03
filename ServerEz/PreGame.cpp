#include "pregame.h"
#include <QDebug>

PreGame::PreGame(int playerCount, QPair<QTcpSocket*, QString> player, QObject *parent)
    : QObject(parent), playerCount(playerCount), waitingPlayers(1)
{
    players.append(player);
    sendSearching();
}

PreGame::~PreGame()
{
    for (auto &player : players) {
        if (player.first && player.first->isOpen()) {
            player.first->write("P[ERROR][Game cancelled due to disconnection]\n");
            player.first->flush();
            player.first->disconnectFromHost();
        }
    }
}

void PreGame::addPlayer(QPair<QTcpSocket*, QString> player)
{
    if (!player.first || !player.first->isOpen()) {
        qDebug() << "Attempted to add invalid client to PreGame";
        return;
    }
    players.append(player);
    ++waitingPlayers;
    if (waitingPlayers < playerCount) {
        sendSearching();
    } else {
        sendFound();
        QStringList playerUsernames;
        for (const auto& player : players) {
            playerUsernames << player.second;
        }
        emit startGame(playerUsernames);
    }
}

void PreGame::sendSearching()
{
    QString msg = QString("P[%1][%2][Searching for a match]\n")
    .arg(playerCount)
        .arg(waitingPlayers);
    for (auto &player : players) {
        if (player.first && player.first->isOpen()) {
            player.first->write(msg.toUtf8());
            player.first->flush();
        }
    }
}

void PreGame::sendFound()
{
    for (auto &me : players) {
        if (!me.first || !me.first->isOpen()) continue;
        QStringList others;
        for (auto &peer : players) {
            if (peer.first && peer.first != me.first && peer.first->isOpen()) {
                others << peer.second;
            }
        }
        QString msg = QString("P[%1][Found]").arg(playerCount);
        for (const auto &name : others) {
            msg += QString("[%1]").arg(name);
        }
        msg += "\n";
        me.first->write(msg.toUtf8());
        me.first->flush();
    }
}
