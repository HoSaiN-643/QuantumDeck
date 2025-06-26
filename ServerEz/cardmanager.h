#ifndef CARDMANAGER_H
#define CARDMANAGER_H

#include <card.h>

class CardManager
{

public:
    CardManager();
    static void Shuffle(QVector<Card>& Cards);
    static QVector<Card> MakeDeck();
    static QVector<Card> Get7Card(QVector<Card>& Cards);

};

#endif // CARDMANAGER_H
