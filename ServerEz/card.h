#ifndef CARD_H
#define CARD_H

#include <QDebug>
#include <QString>
#include <QStringList>

class Card
{
public:
    QString suit;
    int rank;
    Card(QString suit = "", int rank = 0);
    QString Suit() const { return suit; }
    int Rank() const { return rank; }

    static Card fromString(const QString& cardStr);
private:
    static int RankNameToInt(const QString& rankName);
};

#endif // CARD_H
