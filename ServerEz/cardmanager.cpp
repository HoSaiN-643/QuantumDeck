#include "cardmanager.h"
#include <algorithm>
#include <random>
#include <stdexcept>
#include <QMap>
#include <QSet>
#include <QDebug>

CardManager::CardManager() {}

void CardManager::Shuffle(QVector<Card>& Cards)
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(Cards.begin(), Cards.end(), g);
}

QVector<Card> CardManager::MakeDeck()
{
    QVector<Card> Cards;
    QStringList suits = {"Dimond", "Gold", "Dollar", "Coin"};
    for (const auto& suit : suits) {
        for (int rank = 1; rank <= 13; ++rank) {
            Cards.push_back(Card(suit, rank));
        }
    }
    qDebug() << "MakeDeck created" << Cards.size() << "cards";
    return Cards;
}

QVector<Card> CardManager::Get7Card(QVector<Card>& Cards)
{
    if (Cards.size() < 7) {
        qDebug() << "Error: Not enough cards to select 7, deck size:" << Cards.size();
        throw QString("Not enough cards to play");
    }

    QVector<Card> SelectedCards;
    std::random_device rd;
    std::mt19937 g(rd());

    for (int i = 0; i < 7; ++i) {
        std::uniform_int_distribution<> dis(0, Cards.size() - 1);
        int randomIndex = dis(g);
        SelectedCards.append(Cards[randomIndex]);
        Cards.remove(randomIndex);
    }

    return SelectedCards;
}

int CardManager::DetermineHandRank(const QVector<Card>& hand)
{
    if (hand.size() != 5) {
        qDebug() << "Error: Hand must contain exactly 5 cards, got" << hand.size();
        throw std::invalid_argument("Hand must contain exactly 5 cards");
    }

    QVector<Card> sortedHand = hand;
    std::sort(sortedHand.begin(), sortedHand.end(), [](const Card& a, const Card& b) {
        return a.Rank() > b.Rank();
    });

    QMap<int, int> rankCount;
    QMap<QString, int> suitCount;
    for (const auto& card : sortedHand) {
        rankCount[card.Rank()]++;
        suitCount[card.Suit()]++;
    }

    bool isSameSuit = suitCount.size() == 1;
    bool isConsecutive = true;
    for (int i = 1; i < sortedHand.size(); ++i) {
        if (sortedHand[i].Rank() != sortedHand[i-1].Rank() - 1) {
            isConsecutive = false;
            break;
        }
    }

    bool isAceLowStraight = false;
    if (sortedHand[0].Rank() == 1 && sortedHand[1].Rank() == 10 &&
        sortedHand[2].Rank() == 9 && sortedHand[3].Rank() == 8 &&
        sortedHand[4].Rank() == 7) {
        isAceLowStraight = true;
    }

    if (isSameSuit && sortedHand[0].Rank() == 1 && sortedHand[1].Rank() == 13 &&
        sortedHand[2].Rank() == 12 && sortedHand[3].Rank() == 11 && sortedHand[4].Rank() == 10) {
        return 10; // Golden Hand
    }

    if (isSameSuit && (isConsecutive || isAceLowStraight)) {
        return 9; // Order Hand
    }

    for (const auto& count : rankCount) {
        if (count == 4) {
            return 8; // Four to One
        }
    }

    bool hasThree = false, hasPair = false;
    for (const auto& count : rankCount) {
        if (count == 3) hasThree = true;
        else if (count == 2) hasPair = true;
    }
    if (hasThree && hasPair) {
        return 7; // Penthouse
    }

    if (isSameSuit) {
        return 6; // MSC Hand
    }

    if (isConsecutive || isAceLowStraight) {
        return 5; // Series
    }

    if (hasThree) {
        return 4; // Three to Two
    }

    int pairCount = 0;
    for (const auto& count : rankCount) {
        if (count == 2) pairCount++;
    }
    if (pairCount == 2) {
        return 3; // Double Pair
    }

    if (pairCount == 1) {
        return 2; // Single Pair
    }

    return 1; // High Card
}

int CardManager::CompareHands(const QVector<Card>& hand1, const QVector<Card>& hand2)
{
    int rank1 = DetermineHandRank(hand1);
    int rank2 = DetermineHandRank(hand2);

    if (rank1 != rank2) {
        qDebug() << "Comparing hands: rank1 =" << rank1 << ", rank2 =" << rank2;
        return rank1 > rank2 ? 1 : -1;
    }

    QVector<Card> sortedHand1 = hand1;
    QVector<Card> sortedHand2 = hand2;
    std::sort(sortedHand1.begin(), sortedHand1.end(), [](const Card& a, const Card& b) {
        return a.Rank() > b.Rank();
    });
    std::sort(sortedHand2.begin(), sortedHand2.end(), [](const Card& a, const Card& b) {
        return a.Rank() > b.Rank();
    });

    QMap<QString, int> suitRank = {{"Dimond", 4}, {"Gold", 3}, {"Dollar", 2}, {"Coin", 1}};

    switch (rank1) {
    case 10: // Golden Hand
    case 9: // Straight Flush
    case 6: // Flush
    case 5: // Straight
    case 1: // High Card
        for (int i = 0; i < 5; ++i) {
            if (sortedHand1[i].Rank() != sortedHand2[i].Rank()) {
                return sortedHand1[i].Rank() > sortedHand2[i].Rank() ? 1 : -1;
            }
        }
        return suitRank[sortedHand1[0].Suit()] > suitRank[sortedHand2[0].Suit()] ? 1 : -1;
    case 8: // Four of a Kind
    case 7: // Full House
    case 4: // Three of a Kind
    {
        QMap<int, int> rankCount1, rankCount2;
        for (const auto& card : sortedHand1) rankCount1[card.Rank()]++;
        for (const auto& card : sortedHand2) rankCount2[card.Rank()]++;
        int keyRank1 = 0, keyRank2 = 0;
        for (int rank : rankCount1.keys()) {
            if (rankCount1[rank] == 4 || rankCount1[rank] == 3) keyRank1 = rank;
        }
        for (int rank : rankCount2.keys()) {
            if (rankCount2[rank] == 4 || rankCount2[rank] == 3) keyRank2 = rank;
        }
        if (keyRank1 != keyRank2) return keyRank1 > keyRank2 ? 1 : -1;
        for (int i = 0; i < 5; ++i) {
            if (rankCount1[sortedHand1[i].Rank()] <= 1 && rankCount2[sortedHand2[i].Rank()] <= 1) {
                if (sortedHand1[i].Rank() != sortedHand2[i].Rank()) {
                    return sortedHand1[i].Rank() > sortedHand2[i].Rank() ? 1 : -1;
                }
            }
        }
        return 0;
    }
    case 3: // Two Pair
    {
        QVector<int> pairs1, pairs2;
        QMap<int, int> rankCount1, rankCount2;
        for (const auto& card : sortedHand1) rankCount1[card.Rank()]++;
        for (const auto& card : sortedHand2) rankCount2[card.Rank()]++;
        for (int rank : rankCount1.keys()) {
            if (rankCount1[rank] == 2) pairs1.push_back(rank);
        }
        for (int rank : rankCount2.keys()) {
            if (rankCount2[rank] == 2) pairs2.push_back(rank);
        }
        std::sort(pairs1.begin(), pairs1.end(), std::greater<int>());
        std::sort(pairs2.begin(), pairs2.end(), std::greater<int>());
        if (pairs1[0] != pairs2[0]) return pairs1[0] > pairs2[0] ? 1 : -1;
        if (pairs1[1] != pairs2[1]) return pairs1[1] > pairs2[1] ? 1 : -1;
        int kicker1 = 0, kicker2 = 0;
        for (const auto& card : sortedHand1) {
            if (rankCount1[card.Rank()] == 1) kicker1 = card.Rank();
        }
        for (const auto& card : sortedHand2) {
            if (rankCount2[card.Rank()] == 1) kicker2 = card.Rank();
        }
        return kicker1 > kicker2 ? 1 : (kicker1 < kicker2 ? -1 : 0);
    }
    case 2: // One Pair
    {
        int pairRank1 = 0, pairRank2 = 0;
        QMap<int, int> rankCount1, rankCount2;
        for (const auto& card : sortedHand1) rankCount1[card.Rank()]++;
        for (const auto& card : sortedHand2) rankCount2[card.Rank()]++;
        for (int rank : rankCount1.keys()) {
            if (rankCount1[rank] == 2) pairRank1 = rank;
        }
        for (int rank : rankCount2.keys()) {
            if (rankCount2[rank] == 2) pairRank2 = rank;
        }
        if (pairRank1 != pairRank2) return pairRank1 > pairRank2 ? 1 : -1;
        for (int i = 0; i < 5; ++i) {
            if (rankCount1[sortedHand1[i].Rank()] == 1 && rankCount2[sortedHand2[i].Rank()] == 1) {
                if (sortedHand1[i].Rank() != sortedHand2[i].Rank()) {
                    return sortedHand1[i].Rank() > sortedHand2[i].Rank() ? 1 : -1;
                }
            }
        }
        return suitRank[sortedHand1[0].Suit()] > suitRank[sortedHand2[0].Suit()] ? 1 : -1;
    }
    }
    return 0;
}
