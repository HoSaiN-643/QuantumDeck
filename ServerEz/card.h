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

    static Card fromString(const QString& cardStr) {
        QStringList parts = cardStr.split(" of ");
        if (parts.size() != 2) return Card();
        QString rankStr = parts[0];
        QString suit = parts[1];
        int rank = RankNameToInt(rankStr);
        return Card(suit, rank);
    }

private:
    static int RankNameToInt(const QString& rankName) {
        if (rankName == "Ace") return 1;
        if (rankName == "Jack") return 11; // تغییر از Soldier به Jack
        if (rankName == "Queen") return 12;
        if (rankName == "King") return 13;
        bool ok;
        int rank = rankName.toInt(&ok);
        return ok ? rank : -1;
    }
};

#endif // CARD_H
