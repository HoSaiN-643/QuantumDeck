#ifndef PREGAME_H
#define PREGAME_H

#include <QObject>
#include <QTcpSocket>
#include <QVector>
#include <QPair>
#include <QStringList>

class PreGame : public QObject
{
    Q_OBJECT

public:
    explicit PreGame(int playerCount, QPair<QTcpSocket*, QString> player, QObject *parent = nullptr);
    ~PreGame();

    void addPlayer(QPair<QTcpSocket*, QString> player);
    bool isFull() const { return waitingPlayers >= playerCount; }

signals:
    void startGame(const QStringList& players);

private:
    int playerCount;
    int waitingPlayers;
    QVector<QPair<QTcpSocket*, QString>> players;

    void sendSearching();
    void sendFound();
};

#endif // PREGAME_H
