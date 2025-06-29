// pregame.cpp
#include "pregame.h"
#include <QString>

PreGame::PreGame(int count,
                 QPair<QTcpSocket*,QString> p,
                 QObject *parent)
    : QObject(parent),
    PlayerCnt(count)
{
    Players.append(p);
    // نفر اول را هم بشمار
    WaitingPlayers = 1;
    sendSearching();
}

void PreGame::AddPlayer(QPair<QTcpSocket*,QString> p)
{
    // نفر جدید را اضافه کن
    Players.append(p);
    ++WaitingPlayers;

    if (WaitingPlayers < PlayerCnt) {
        sendSearching();
    }
    else {
        // پر شد
        IsServerFull = true;
        sendFound();
    }
}

void PreGame::sendSearching()
{
    // پیام: P[PlayerCnt][WaitingPlayers][Searching for a match]
    QString msg = QString("P[%1][%2][Searching for a match]")
                      .arg(QString::number(PlayerCnt),
                           QString::number(WaitingPlayers));
    QByteArray raw = msg.toUtf8();
    for (auto &pr : Players) {
        pr.first->write(raw);
    }
}

void PreGame::sendFound()
{
    // پیام: P[PlayerCnt][Found][<نام دیگران>...]
    for (auto &me : Players) {
        // جمع‌کردن نام‌های بقیه
        QStringList others;
        for (auto &peer : Players) {
            if (peer.first != me.first)
                others << peer.second;
        }
        // ساخت رشته
        QString msg = QString("P[%1][Found]")
                          .arg(QString::number(PlayerCnt));
        for (auto &name : others)
            msg += QString("[%1]").arg(name);
        me.first->write(msg.toUtf8());
    }
}
