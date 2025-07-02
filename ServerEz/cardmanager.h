// cardmanager.h
#ifndef CARDMANAGER_H
#define CARDMANAGER_H

#include <QVector>
#include <card.h>

class CardManager
{
public:
    CardManager();
    static void Shuffle(QVector<Card>& Cards);
    static QVector<Card> MakeDeck();
    static QVector<Card> Get7Card(QVector<Card>& Cards);
    static int CompareHands(const QVector<Card>& hand1, const QVector<Card>& hand2);
    static int DetermineHandRank(const QVector<Card>& hand);
};

#endif // CARDMANAGER_H
