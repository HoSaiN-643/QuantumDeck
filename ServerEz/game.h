#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QTcpSocket>
#include <QVector>
#include <QMap>
#include <QTimer>
#include "cardmanager.h"
#include "memberdatabasemanager.h"

class Game : public QObject
{
    Q_OBJECT

public:
    explicit Game(const QStringList &players, QMap<QTcpSocket*, QString> &clients,
                  MemberDatabaseManager &db, QObject *parent = nullptr);
    ~Game();

    void start();
    void processChatMessage(QTcpSocket *client, const QStringList &fields);

private:
    QStringList playerUsernames;
    QMap<QTcpSocket*, QString> &clients;
    MemberDatabaseManager &db;
    QVector<Card> player1Cards;
    QVector<Card> player2Cards;
    int player1Wins;
    int player2Wins;
    int currentRound;
    QVector<Card> currentDeck;
    QVector<Card> communityCards;
    QTimer *cardViewTimer;
    int startingPlayerIndex;
    bool waitingForPlayer1;
    bool waitingForPlayer2;
    int currentTurn;

    void startRound();
    void sendDiamondCards();
    void sendCommunityCards();
    void processPlayerCardChoice(QTcpSocket *client, const QStringList &fields);
    void determineRoundWinner();
    void checkGameWinner();
    void resetRound();

signals:
    void gameFinished();

private slots:
    void onCardViewTimeout();
    void handleClientMessage();
};

#endif // GAME_H
