#include "game.h"
#include <QDebug>
#include <QTimer>

Game::Game(const QStringList &players, QMap<QTcpSocket*, QString> &clients, QObject *parent)
    : QObject(parent), playerUsernames(players), clients(clients), player1Wins(0), player2Wins(0),
    currentRound(0), cardViewTimer(new QTimer(this)), startingPlayerIndex(0),
    waitingForPlayer1(false), waitingForPlayer2(false), currentTurn(0)
{
    connect(cardViewTimer, &QTimer::timeout, this, &Game::onCardViewTimeout);
}

void Game::start()
{
    if (playerUsernames.size() != 2) {
        qDebug() << "Error: Game requires exactly 2 players";
        emit gameFinished();
        return;
    }
    qDebug() << "Starting game with players:" << playerUsernames;

    // انتخاب اولیه بازیکن شروع‌کننده با کارت‌های Diamond
    currentDeck = CardManager::MakeDeck();
    CardManager::Shuffle(currentDeck);
    sendDiamondCards();
}

void Game::startRound()
{
    if (currentRound >= 3 || player1Wins >= 2 || player2Wins >= 2) {
        checkGameWinner();
        return;
    }

    currentRound++;
    player1Cards.clear();
    player2Cards.clear();
    communityCards.clear();
    currentTurn = 0;
    waitingForPlayer1 = false;
    waitingForPlayer2 = false;

    // ساخت و شافل دسته کارت
    currentDeck = CardManager::MakeDeck();
    CardManager::Shuffle(currentDeck);

    // تعیین بازیکن شروع‌کننده بر اساس راند
    waitingForPlayer1 = (startingPlayerIndex == (currentRound % 2));
    waitingForPlayer2 = !waitingForPlayer1;
    sendCommunityCards();
}

void Game::sendDiamondCards()
{
    // انتخاب 2 کارت رندوم از خال Diamond
    QVector<Card> diamondCards;
    for (const auto &card : currentDeck) {
        if (card.Suit() == "Diamond") {
            diamondCards.append(card);
        }
    }
    CardManager::Shuffle(diamondCards);

    if (diamondCards.size() < 2) {
        qDebug() << "Error: Not enough Diamond cards";
        QString errorMsg = "G[ERROR][Not enough Diamond cards]\n";
        for (auto client : clients.keys()) {
            if (clients[client] == playerUsernames[0] || clients[client] == playerUsernames[1]) {
                client->write(errorMsg.toUtf8());
                client->flush();
            }
        }
        emit gameFinished();
        return;
    }

    // ارسال کارت به بازیکن اول
    QString player1Message = QString("G[ST][Diamond-%1]\n").arg(diamondCards[0].Rank());
    for (auto client : clients.keys()) {
        if (clients[client] == playerUsernames[0]) {
            client->write(player1Message.toUtf8());
            client->flush();
            break;
        }
    }

    // ارسال کارت به بازیکن دوم
    QString player2Message = QString("G[ST][Diamond-%1]\n").arg(diamondCards[1].Rank());
    for (auto client : clients.keys()) {
        if (clients[client] == playerUsernames[1]) {
            client->write(player2Message.toUtf8());
            client->flush();
            break;
        }
    }

    // حذف کارت‌های استفاده‌شده از دسته
    for (int i = currentDeck.size() - 1; i >= 0; --i) {
        if (currentDeck[i].Suit() == "Diamond" &&
            (currentDeck[i].Rank() == diamondCards[0].Rank() || currentDeck[i].Rank() == diamondCards[1].Rank())) {
            currentDeck.remove(i);
        }
    }

    // تعیین بازیکن شروع‌کننده برای کل بازی
    startingPlayerIndex = (diamondCards[0].Rank() > diamondCards[1].Rank()) ? 0 : 1;
    waitingForPlayer1 = (startingPlayerIndex == 0);
    waitingForPlayer2 = !waitingForPlayer1;

    // شروع تایمر 5 ثانیه
    cardViewTimer->start(5000);
}

void Game::onCardViewTimeout()
{
    cardViewTimer->stop();
    startRound();
}

void Game::sendCommunityCards()
{
    if (player1Cards.size() >= 5 && player2Cards.size() >= 5) {
        determineRoundWinner();
        return;
    }

    // انتخاب 7 کارت رندوم
    communityCards = CardManager::Get7Card(currentDeck);
    QString message = "G[CC]";
    for (const auto &card : communityCards) {
        message += QString("[%1-%2]").arg(card.Suit(), QString::number(card.Rank()));
    }
    message += "\n";

    // ارسال به بازیکنی که نوبتش است
    QString currentPlayer = waitingForPlayer1 ? playerUsernames[0] : playerUsernames[1];
    for (auto client : clients.keys()) {
        if (clients[client] == currentPlayer) {
            client->write(message.toUtf8());
            client->flush();
            break;
        }
    }
}

void Game::processPlayerCardChoice(QTcpSocket *client, const QStringList &fields)
{


    // بررسی نوبت
    bool isPlayer1 = (clients[client] == playerUsernames[0]);
    if ((isPlayer1 && !waitingForPlayer1) || (!isPlayer1 && !waitingForPlayer2)) {
        qDebug() << "Not your turn:" << clients[client];
        return;
    }

    // پردازش کارت انتخاب‌شده
    QString cardStr = fields[1]; // فرمت: suit-rank
    QStringList parts = cardStr.split("-");
    if (parts.size() != 2) {
        qDebug() << "Invalid card format from" << clients[client];
        return;
    }
    Card selectedCard(parts[0], parts[1].toInt());

    // بررسی وجود کارت در کارت‌های مشترک
    bool cardFound = false;
    for (int i = 0; i < communityCards.size(); ++i) {
        if (communityCards[i].Suit() == selectedCard.Suit() && communityCards[i].Rank() == selectedCard.Rank()) {
            communityCards.remove(i);
            cardFound = true;
            break;
        }
    }
    if (!cardFound) {
        qDebug() << "Card not found in community cards:" << cardStr;
        return;
    }

    // اضافه کردن کارت به لیست بازیکن
    if (isPlayer1) {
        player1Cards.append(selectedCard);
        waitingForPlayer1 = false;
        waitingForPlayer2 = true;
    } else {
        player2Cards.append(selectedCard);
        waitingForPlayer2 = false;
        waitingForPlayer1 = true;
    }

    // ارسال 6 کارت باقی‌مانده به بازیکن دیگر
    if (communityCards.size() == 6) {
        QString message = "G[CC]";
        for (const auto &card : communityCards) {
            message += QString("[%1-%2]").arg(card.Suit(), QString::number(card.Rank()));
        }
        message += "\n";
        QString nextPlayer = waitingForPlayer1 ? playerUsernames[0] : playerUsernames[1];
        for (auto client : clients.keys()) {
            if (clients[client] == nextPlayer) {
                client->write(message.toUtf8());
                client->flush();
                break;
            }
        }
    } else if (communityCards.size() == 5) {
        // حذف کل 7 کارت از دسته
        for (const auto &card : communityCards) {
            for (int i = currentDeck.size() - 1; i >= 0; --i) {
                if (currentDeck[i].Suit() == card.Suit() && currentDeck[i].Rank() == card.Rank()) {
                    currentDeck.remove(i);
                    break;
                }
            }
        }
        // حذف کارت‌های انتخاب‌شده
        for (const auto &card : {player1Cards.last(), player2Cards.last()}) {
            for (int i = currentDeck.size() - 1; i >= 0; --i) {
                if (currentDeck[i].Suit() == card.Suit() && currentDeck[i].Rank() == card.Rank()) {
                    currentDeck.remove(i);
                    break;
                }
            }
        }
        communityCards.clear();
        sendCommunityCards();
    }
}

void Game::determineRoundWinner()
{
    int result = CardManager::CompareHands(player1Cards, player2Cards);
    QString winnerUsername;
    if (result > 0) {
        player1Wins++;
        winnerUsername = playerUsernames[0];
    } else if (result < 0) {
        player2Wins++;
        winnerUsername = playerUsernames[1];
    } else {
        winnerUsername = "Tie";
    }

    // ارسال نتیجه راند
    QString message = QString("G[RF][%1]\n").arg(winnerUsername);
    for (auto client : clients.keys()) {
        if (clients[client] == playerUsernames[0] || clients[client] == playerUsernames[1]) {
            client->write(message.toUtf8());
            client->flush();
        }
    }

    // شروع راند بعدی یا پایان بازی
    startRound();
}

void Game::checkGameWinner()
{
    QString winner;
    if (player1Wins >= 2) {
        winner = playerUsernames[0];
    } else if (player2Wins >= 2) {
        winner = playerUsernames[1];
    } else {
        winner = "Tie";
    }

    // ارسال پیام پایان بازی
    QString message = QString("G[END][%1]\n").arg(winner);
    for (auto client : clients.keys()) {
        if (clients[client] == playerUsernames[0] || clients[client] == playerUsernames[1]) {
            client->write(message.toUtf8());
            client->flush();
        }
    }

    emit gameFinished();
}

void Game::resetRound()
{
    currentDeck = CardManager::MakeDeck();
    CardManager::Shuffle(currentDeck);
    player1Cards.clear();
    player2Cards.clear();
    communityCards.clear();
    currentTurn = 0;
}

void Game::handleClientMessage(QTcpSocket *client, const QStringList &fields)
{
    if (fields.isEmpty()) return;
    if (fields[0] == "CC") {
        processPlayerCardChoice(client, fields);
    }
}
