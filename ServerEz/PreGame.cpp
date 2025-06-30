// pregame.cpp
#include "pregame.h"
#include <QString>

PreGame::PreGame(int count,
                 QPair<QTcpSocket*,QString> p,
                 QObject *parent)
    : QObject(parent),
    PlayerCnt(count),
    WaitingPlayers(1)
{
    Players.append(p);
    sendSearching();
}

void PreGame::AddPlayer(QPair<QTcpSocket*,QString> p)
{
    Players.append(p);
    ++WaitingPlayers;
    if (WaitingPlayers < PlayerCnt)
        sendSearching();
    else {
        IsServerFull = true;
        sendFound();
    }
}

void PreGame::sendSearching()
{
    // پیام: P[count][waiting][Searching for a match
    QString msg = QString("P[%1][%2][Searching for a match]")
                      .arg(PlayerCnt)
                      .arg(WaitingPlayers);
    QByteArray raw = msg.toUtf8();
    for (auto &pr : Players)
        pr.first->write(raw);
}

void PreGame::sendFound()
{
    // هر بازیکن، لیست بقیه را می‌گیرد
    for (auto &me : Players) {
        QStringList others;
        for (auto &peer : Players)
            if (peer.first != me.first)
                others << peer.second;

        QString msg = QString("P[%1][Found]").arg(PlayerCnt);
        for (auto &name : others)
            msg += QString("[%1]").arg(name);
        msg += "\n";

        me.first->write(msg.toUtf8());
    }
}

