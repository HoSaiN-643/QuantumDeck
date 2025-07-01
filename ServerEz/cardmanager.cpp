#include "cardmanager.h"
#include <card.h>
#include <algorithm>
#include <QMap>
#include <QMapIterator>
#include <QSet>
#include <stdexcept>

CardManager::CardManager() {}

void CardManager::Shuffle(QVector<Card>& Cards)
{
    std::srand(std::time(nullptr));
    std::random_shuffle(Cards.begin(), Cards.end());
}

QVector<Card> CardManager::MakeDeck()
{
    QVector<Card> Cards;
    QStringList suits = {"Diamond", "Gold", "Dollar", "Coin"};

    for (const auto& suit : suits) {
        for (int rank = 1; rank <= 13; ++rank) {
            Cards.push_back(Card(suit, rank));
        }
    }
    return Cards;
}

QVector<Card> CardManager::Get7Card(QVector<Card>& Cards)
{
    if (Cards.size() < 7) {
        throw std::invalid_argument("Not enough cards to play");
    }
    QVector<Card> Deck;
    for (int i = 0; i < 7; ++i) {
        Deck.append(Cards.front());
        Cards.pop_front();
    }
    return Deck;
}

int CardManager::DetermineHandRank(const QVector<Card>& hand)
{
    if (hand.size() != 5) {
        throw std::invalid_argument("Hand must contain exactly 5 cards");
    }

    // Sort hand by rank in descending order
    QVector<Card> sortedHand = hand;
    std::sort(sortedHand.begin(), sortedHand.end(), [](const Card& a, const Card& b) {
        return a.Rank() > b.Rank();
    });

    // Count ranks and suits
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

    // Check for Golden Hand (A=13, K=12, Q=11, J=10, 10=9)
    if (isSameSuit && sortedHand[0].Rank() == 13 && sortedHand[1].Rank() == 12 &&
        sortedHand[2].Rank() == 11 && sortedHand[3].Rank() == 10 && sortedHand[4].Rank() == 9) {
        return 10; // Golden Hand
    }

    // Check for Order Hand (Straight Flush)
    if (isSameSuit && isConsecutive) {
        return 9; // Order Hand
    }

    // Check for Four to One (Four of a Kind)
    for (auto it = rankCount.begin(); it != rankCount.end(); ++it) {
        if (it.value() == 4) {
            return 8; // Four to One
        }
    }

    // Check for Penthouse (Three from three suits + Pair from two suits)
    bool hasThree = false, hasPair = false;
    int threeRank = 0, pairRank = 0;
    for (auto it = rankCount.begin(); it != rankCount.end(); ++it) {
        if (it.value() == 3) {
            hasThree = true;
            threeRank = it.key();
        } else if (it.value() == 2) {
            hasPair = true;
            pairRank = it.key();
        }
    }
    if (hasThree && hasPair) {
        QSet<QString> threeSuits, pairSuits;
        for (const auto& card : hand) {
            if (card.Rank() == threeRank) threeSuits.insert(card.Suit());
            if (card.Rank() == pairRank) pairSuits.insert(card.Suit());
        }
        if (threeSuits.size() == 3 && pairSuits.size() == 2) {
            return 7; // Penthouse
        }
    }

    // Check for MSC Hand (Flush)
    if (isSameSuit) {
        return 6; // MSC Hand
    }

    // Check for Series (Straight)
    if (isConsecutive) {
        return 5; // Series
    }

    // Check for Three to Two (Three of a Kind)
    if (hasThree) {
        QSet<QString> threeSuits;
        for (const auto& card : hand) {
            if (card.Rank() == threeRank) threeSuits.insert(card.Suit());
        }
        if (threeSuits.size() == 3) {
            return 4; // Three to Two
        }
    }

    // Check for Double Pair
    int pairCount = 0;
    for (auto it = rankCount.begin(); it != rankCount.end(); ++it) {
        if (it.value() == 2) pairCount++;
    }
    if (pairCount == 2) {
        return 3; // Double Pair
    }

    // Check for Single Pair
    if (pairCount == 1) {
        return 2; // Single Pair
    }

    // Messy Hand (High Card)
    return 1;
}

int CardManager::CompareHands(const QVector<Card>& hand1, const QVector<Card>& hand2)
{
    int rank1 = DetermineHandRank(hand1);
    int rank2 = DetermineHandRank(hand2);

    if (rank1 != rank2) {
        return rank1 > rank2 ? 1 : -1;
    }

    // Sort hands for comparison
    QVector<Card> sortedHand1 = hand1;
    QVector<Card> sortedHand2 = hand2;
    std::sort(sortedHand1.begin(), sortedHand1.end(), [](const Card& a, const Card& b) {
        return a.Rank() > b.Rank();
    });
    std::sort(sortedHand2.begin(), sortedHand2.end(), [](const Card& a, const Card& b) {
        return a.Rank() > b.Rank();
    });

    // Define suit ranking
    QMap<QString, int> suitRank = {{"Diamond", 4}, {"Gold", 3}, {"Dollar", 2}, {"Coin", 1}};

    // Compare based on hand rank rules
    switch (rank1) {
    case 10: // Golden Hand
        return suitRank[sortedHand1[0].Suit()] > suitRank[sortedHand2[0].Suit()] ? 1 : -1;
    case 9: // Order Hand
    case 5: // Series
        if (sortedHand1[0].Rank() != sortedHand2[0].Rank()) {
            return sortedHand1[0].Rank() > sortedHand2[0].Rank() ? 1 : -1;
        }
        return suitRank[sortedHand1[0].Suit()] > suitRank[sortedHand2[0].Suit()] ? 1 : -1;
    case 8: // Four to One
    case 7: // Penthouse
    case 4: // Three to Two
        {
            QMap<int, int> rankCount1, rankCount2;
            for (const auto& card : sortedHand1) rankCount1[card.Rank()]++;
            for (const auto& card : sortedHand2) rankCount2[card.Rank()]++;
            int rank1Four = 0, rank2Four = 0, rank1Three = 0, rank2Three = 0;
            for (auto it = rankCount1.begin(); it != rankCount1.end(); ++it) {
                if (it.value() == 4) rank1Four = it.key();
                else if (it.value() == 3) rank1Three = it.key();
            }
            for (auto it = rankCount2.begin(); it != rankCount2.end(); ++it) {
                if (it.value() == 4) rank2Four = it.key();
                else if (it.value() == 3) rank2Three = it.key();
            }
            int rankToCompare1 = rank1Four ? rank1Four : rank1Three;
            int rankToCompare2 = rank2Four ? rank2Four : rank2Three;
            return rankToCompare1 > rankToCompare2 ? 1 : (rankToCompare1 < rankToCompare2 ? -1 : 0);
        }
    case 6: // MSC Hand
    case 1: // Messy Hand
        for (int i = 0; i < 5; ++i) {
            if (sortedHand1[i].Rank() != sortedHand2[i].Rank()) {
                return sortedHand1[i].Rank() > sortedHand2[i].Rank() ? 1 : -1;
            }
        }
        return suitRank[sortedHand1[0].Suit()] > suitRank[sortedHand2[0].Suit()] ? 1 : -1;
    case 3: // Double Pair
        {
            QVector<int> pairs1, pairs2;
            QMap<int, int> rankCount1, rankCount2;
            for (const auto& card : sortedHand1) rankCount1[card.Rank()]++;
            for (const auto& card : sortedHand2) rankCount2[card.Rank()]++;
            for (auto it = rankCount1.begin(); it != rankCount1.end(); ++it) {
                if (it.value() == 2) pairs1.push_back(it.key());
            }
            for (auto it = rankCount2.begin(); it != rankCount2.end(); ++it) {
                if (it.value() == 2) pairs2.push_back(it.key());
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
    case 2: // Single Pair
        {
            int pairRank1 = 0, pairRank2 = 0;
            QMap<int, int> rankCount1, rankCount2;
            for (const auto& card : sortedHand1) rankCount1[card.Rank()]++;
            for (const auto& card : sortedHand2) rankCount2[card.Rank()]++;
            for (auto it = rankCount1.begin(); it != rankCount1.end(); ++it) {
                if (it.value() == 2) pairRank1 = it.key();
            }
            for (auto it = rankCount2.begin(); it != rankCount2.end(); ++it) {
                if (it.value() == 2) pairRank2 = it.key();
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
    return 0; // Tie
}