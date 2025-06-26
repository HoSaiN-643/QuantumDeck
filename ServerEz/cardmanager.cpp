
#include "cardmanager.h"
#include <card.h>

CardManager::CardManager() {}

void CardManager::Shuffle(QVector<Card>& Cards)
{
    std::srand(std::time(nullptr));

    std::random_shuffle(Cards.begin(),Cards.end());
}

QVector<Card> CardManager::MakeDeck()
{
    QVector<Card> Cards;
    QStringList suits = {"Diamond","Gold","Dollar","Coin"};

    for(auto suit : suits ) {
        for (int rank = 1; rank <= 13;++rank) {
                Cards.push_back(Card(suit,rank));
        }
    }

        return Cards;
}

QVector<Card> CardManager::Get7Card(QVector<Card>& Cards)
{
    if(Cards.size() < 7) {
        throw "Not enough cards to play";

    }
    QVector<Card> Deck;
    int Cnt = 0;
    while(Cnt < 8) {
        Deck.append(Cards.front());
        Cards.pop_front();
    }
    return Deck;


}
