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
    bool IsServerFull{false};

    explicit PreGame(int count,
                     QPair<QTcpSocket*, QString> p,
                     QObject *parent = nullptr);

    void AddPlayer(QPair<QTcpSocket*, QString> p);

private:
    int PlayerCnt{0};
    int WaitingPlayers{0};
    QVector<QPair<QTcpSocket*, QString>> Players;

    void sendSearching();
    void sendFound();
};

#endif // PREGAME_H
