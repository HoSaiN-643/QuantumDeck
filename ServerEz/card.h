#ifndef CARD_H
#define CARD_H

#include <QDebug>

class Card
{
    QString suit;
    int rank;

public:
    Card(QString,int);
    QString Suit() const
    {
        return suit;
    }
    int Rank() const
    {
        return rank;
    }
    QString description() {
        QString RankStr;

        switch(rank) {

        case 1  : RankStr = "Ace"; break;
        case 11 : RankStr = "Soldier"; break;
        case 12 : RankStr = "Queen"; break;
        case 13 : RankStr = "King"; break;
        default : RankStr = QString::number(rank);

        }

        return QString("%1 of %2").arg(RankStr,suit);

    }


};

#endif // CARD_H
