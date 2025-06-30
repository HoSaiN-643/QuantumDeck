#include "pregame.h"
#include <QString>

PreGame::PreGame(int count,
                 QPair<QTcpSocket*, QString> p,
                 QObject *parent)
    : QObject(parent),
    PlayerCnt(count),
    WaitingPlayers(1)
{
    Players.append(p);
    sendSearching();
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
    }
}

void PreGame::sendSearching()
{
    QString msg = QString("P[%1][%2][Searching for a match]\n")
    .arg(PlayerCnt)
        .arg(WaitingPlayers);
    QByteArray raw = msg.toUtf8();
    for (auto &pr : Players) {
        if (pr.first) {
            pr.first->write(raw);
        }
    }
}

void PreGame::sendFound()
{
    for (auto &me : Players) {
        if (!me.first) continue;
        QStringList others;
        for (auto &peer : Players) {
            if (peer.first && peer.first != me.first) {
                others << peer.second;
            }
        }
        QString msg = QString("P[%1][Found]").arg(PlayerCnt);
        for (const auto &name : others) {
            msg += QString("[%1]").arg(name);
        }
        msg += "\n";
        me.first->write(msg.toUtf8());
    }
}
