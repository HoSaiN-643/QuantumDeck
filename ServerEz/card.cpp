#include "card.h"

Card::Card(QString suit, int rank) : suit(suit), rank(rank) {}

Card Card::fromString(const QString& cardStr) {
    QStringList parts = cardStr.split(" of ");
    if (parts.size() != 2) return Card();
    QString rankStr = parts[0];
    QString suit = parts[1];
    int rank = RankNameToInt(rankStr);
    return Card(suit, rank);
}

int Card::RankNameToInt(const QString& rankName) {
    if (rankName == "Bitcoin") return 1;
    if (rankName == "Soldier") return 11;
    if (rankName == "Queen") return 12;
    if (rankName == "King") return 13;
    bool ok;
    int rank = rankName.toInt(&ok);
    return ok ? rank : -1;
}
