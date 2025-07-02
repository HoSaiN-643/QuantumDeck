#include "pregame.h"
#include <QString>
#include <QDebug>

PreGame::PreGame(int count, QPair<QTcpSocket*, QString> p, QObject *parent)
    : QObject(parent), PlayerCnt(count), WaitingPlayers(1)
{
    Players.append(p);
    sendSearching();
}

PreGame::~PreGame()
{
    for (auto &player : Players) {
        if (player.first) {
            player.first->disconnect(); // قطع اتصال سوکت‌ها
        }
    }
}

void PreGame::AddPlayer(QPair<QTcpSocket*, QString> p)
{
    if (!p.first) {
        qDebug() << "Attempted to add null client to PreGame";
        return;
    }
    Players.append(p);
    ++WaitingPlayers;
    if (WaitingPlayers < PlayerCnt) {
        sendSearching();
    } else {
        IsServerFull = true;
        sendFound();
        QStringList playerIds;
        for (const auto& player : Players) {
            playerIds << player.second;
        }
        emit startGame(playerIds);
    }
}

void PreGame::sendSearching()
{
    QString msg = QString("P[%1][%2][Searching for a match]\n")
    .arg(PlayerCnt)
        .arg(WaitingPlayers);
    QByteArray raw = msg.toUtf8();
    for (auto &pr : Players) {
        if (pr.first && pr.first->isOpen()) {
            pr.first->write(raw);
            pr.first->flush();
        }
    }
}

void PreGame::sendFound()
{
    for (auto &me : Players) {
        if (!me.first || !me.first->isOpen()) continue;
        QStringList others;
        for (auto &peer : Players) {
            if (peer.first && peer.first != me.first && peer.first->isOpen()) {
                others << peer.second;
            }
        }
        QString msg = QString("P[%1][Found]").arg(PlayerCnt);
        for (const auto &name : others) {
            msg += QString("[%1]").arg(name);
        }
        msg += "\n";
        me.first->write(msg.toUtf8());
        me.first->flush();
    }
}
