#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <card.h>
#include <QTcpSocket>
#include <cardmanager.h>

class Game : QObject
{
    Q_OBJECT
    int round;
    int PlayerCnt;
    int WaitingPlayers;
    QVector<Card> Cards;
    QVector<QTcpSocket*> Players;
    bool IsServerFull;


public:
    Game(int Cnt,QTcpSocket* Player);
    void AddPlayer(QTcpSocket* p);
    void InCreaseWaitingPlayers()
    {
        ++WaitingPlayers;
    }
};

#endif // GAME_H
