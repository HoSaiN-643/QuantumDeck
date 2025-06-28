#ifndef PREGAME_H
#define PREGAME_H

#include <QObject>
#include <card.h>
#include <QTcpSocket>
#include <cardmanager.h>

class PreGame : QObject
{
    Q_OBJECT
    int round;
    int PlayerCnt;
    int WaitingPlayers;
    QVector<Card> Cards;
    QVector<QTcpSocket*> Players;
    bool IsServerFull;


public:
    PreGame(int Cnt,QTcpSocket* Player);
    void AddPlayer(QTcpSocket* p);
    void InCreaseWaitingPlayers()
    {
        ++WaitingPlayers;
    }
};

#endif // PREGAME_H
