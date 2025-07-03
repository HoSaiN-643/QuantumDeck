#include "game.h"
#include <QDebug>
#include <QTimer>

Game::Game(const QStringList &players, QMap<QTcpSocket*, QString> &clients,
           MemberDatabaseManager &db, QObject *parent)
    : QObject(parent), playerUsernames(players), clients(clients), db(db),
    player1Wins(0), player2Wins(0), currentRound(0), cardViewTimer(new QTimer(this)),
    startingPlayerIndex(0), waitingForPlayer1(false), waitingForPlayer2(false), currentTurn(0)
{
    connect(cardViewTimer, &QTimer::timeout, this, &Game::onCardViewTimeout);
    for (auto client : clients.keys()) {
        connect(client, &QTcpSocket::readyRead, this, &Game::handleClientMessage);
    }
}

Game::~Game()
{
    delete cardViewTimer;
}

void Game::start()
{
    if (playerUsernames.size() != 2) {
        qDebug() << "Error: Game requires exactly 2 players";
        emit gameFinished();
        return;
    }
    qDebug() << "Starting game with players:" << playerUsernames;
    currentDeck = CardManager::MakeDeck();
    qDebug() << "Deck created with" << currentDeck.size() << "cards";
    CardManager::Shuffle(currentDeck);
    sendDiamondCards();
}

void Game::sendDiamondCards()
{
    QVector<Card> diamondCards;
    for (const auto &card : currentDeck) {
        if (card.Suit() == "Dimond") {
            diamondCards.append(card);
        }
    }
    qDebug() << "Found" << diamondCards.size() << "Dimond cards";

    if (diamondCards.size() < 2) {
        qDebug() << "Error: Not enough Dimond cards in deck";
        QString errorMsg = "G[ERROR][Not enough Dimond cards]\n";
        for (auto client : clients.keys()) {
            if (clients[client] == playerUsernames[0] || clients[client] == playerUsernames[1]) {
                client->write(errorMsg.toUtf8());
                client->flush();
            }
        }
        emit gameFinished();
        return;
    }

    CardManager::Shuffle(diamondCards);

    QString player1Message = QString("G[ST][YOU][Dimond-%1][OTHER][Dimond-%2]\n")
                                 .arg(diamondCards[0].Rank())
                                 .arg(diamondCards[1].Rank());
    for (auto client : clients.keys()) {
        if (clients[client] == playerUsernames[0]) {
            client->write(player1Message.toUtf8());
            client->flush();
            qDebug() << "Sent to player1 (" << playerUsernames[0] << "):" << player1Message;
            break;
        }
    }

    QString player2Message = QString("G[ST][YOU][Dimond-%1][OTHER][Dimond-%2]\n")
                                 .arg(diamondCards[1].Rank())
                                 .arg(diamondCards[0].Rank());
    for (auto client : clients.keys()) {
        if (clients[client] == playerUsernames[1]) {
            client->write(player2Message.toUtf8());
            client->flush();
            qDebug() << "Sent to player2 (" << playerUsernames[1] << "):" << player2Message;
            break;
        }
    }

    for (int i = 0; i < currentDeck.size(); ++i) {
        if (currentDeck[i].Suit() == "Dimond" &&
            (currentDeck[i].Rank() == diamondCards[0].Rank() || currentDeck[i].Rank() == diamondCards[1].Rank())) {
            currentDeck.remove(i);
            --i;
        }
    }

    int player1Rank = (diamondCards[0].Rank() == 1) ? 14 : diamondCards[0].Rank();
    int player2Rank = (diamondCards[1].Rank() == 1) ? 14 : diamondCards[1].Rank();
    startingPlayerIndex = (player1Rank > player2Rank) ? 0 : 1;
    waitingForPlayer1 = (startingPlayerIndex == 0);
    waitingForPlayer2 = !waitingForPlayer1;
    qDebug() << "Player1 card rank:" << player1Rank << ", Player2 card rank:" << player2Rank;
    qDebug() << "Starting player set to:" << playerUsernames[startingPlayerIndex];

    cardViewTimer->start(5000);
}

void Game::sendCommunityCards()
{
    if (player1Cards.size() < 5 || player2Cards.size() < 5) {
        communityCards = CardManager::Get7Card(currentDeck);
        qDebug() << "Selected" << communityCards.size() << "community cards";

        QString currentPlayer = waitingForPlayer1 ? playerUsernames[0] : playerUsernames[1];
        QString message = QString("G[CC][YOU]");
        for (const auto &card : communityCards) {
            message += QString("[%1-%2]").arg(card.Suit(), QString::number(card.Rank()));
        }
        message += "\n";
        for (auto client : clients.keys()) {
            if (clients[client] == currentPlayer) {
                client->write(message.toUtf8());
                client->flush();
                qDebug() << "Sent community cards to" << currentPlayer << ":" << message;
                break;
            }
        }

        QString otherPlayer = waitingForPlayer1 ? playerUsernames[1] : playerUsernames[0];
        QString waitMessage = QString("G[CC][WAIT][%1]").arg(communityCards.size());
        waitMessage += "\n";
        for (auto client : clients.keys()) {
            if (clients[client] == otherPlayer) {
                client->write(waitMessage.toUtf8());
                client->flush();
                qDebug() << "Sent wait message to" << otherPlayer << ":" << waitMessage;
                break;
            }
        }
    } else {
        determineRoundWinner();
    }
}

void Game::processPlayerCardChoice(QTcpSocket *client, const QStringList &fields)
{
    if (fields.size() != 2 || fields[0] != "CC") {
        qDebug() << "Invalid card choice from" << clients[client];
        return;
    }

    bool isPlayer1 = (clients[client] == playerUsernames[0]);
    if ((isPlayer1 && !waitingForPlayer1) || (!isPlayer1 && !waitingForPlayer2)) {
        qDebug() << "Not your turn:" << clients[client];
        return;
    }

    QString cardStr = fields[1];
    QStringList parts = cardStr.split("-");
    if (parts.size() != 2) {
        qDebug() << "Invalid card format from" << clients[client];
        return;
    }

    Card selectedCard(parts[0], parts[1].toInt());
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

    if (isPlayer1) {
        player1Cards.append(selectedCard);
        waitingForPlayer1 = false;
        waitingForPlayer2 = true;
    } else {
        player2Cards.append(selectedCard);
        waitingForPlayer2 = false;
        waitingForPlayer1 = true;
    }

    if (communityCards.size() > 0) {
        sendCommunityCards();
    }
}

void Game::processChatMessage(QTcpSocket *client, const QStringList &fields)
{
    if (fields.isEmpty()) {
        qDebug() << "Invalid chat message from" << clients[client] << "with no fields";
        return;
    }

    QString senderUsername = clients[client];
    QString message = fields.join(" ");
    if (message.isEmpty()) {
        qDebug() << "Empty message from" << senderUsername;
        return;
    }

    QString chatMessage = QString("G[CHAT][%1][%2]").arg(senderUsername, message);
    chatMessage += "\n";
    qDebug() << "Broadcasting chat message:" << chatMessage;

    for (auto client : clients.keys()) {
        if (clients[client] == playerUsernames[0] || clients[client] == playerUsernames[1]) {
            client->write(chatMessage.toUtf8());
            client->flush();
            qDebug() << "Sent chat to" << clients[client];
        }
    }
}

void Game::determineRoundWinner()
{
    if (player1Cards.size() != 5 || player2Cards.size() != 5) {
        qDebug() << "Error: Not enough cards to determine winner";
        return;
    }

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

    QString message = QString("G[RF][%1]").arg(winnerUsername);
    message += "\n";
    for (auto client : clients.keys()) {
        if (clients[client] == playerUsernames[0] || clients[client] == playerUsernames[1]) {
            client->write(message.toUtf8());
            client->flush();
        }
    }

    QTimer::singleShot(5000, this, &Game::startRound);
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

    if (!winner.isEmpty() && winner != "Tie") {
        for (const auto &username : playerUsernames) {
            db.addGameHistory(username, playerUsernames[0], playerUsernames[1], winner);
        }

        for (const auto &username : playerUsernames) {
            QString historyMsg = "L[HIST]";
            QList<QVariantMap> history = db.getGameHistory(username);
            int count = 0;
            for (auto it = history.rbegin(); it != history.rend() && count < 3; ++it, ++count) {
                historyMsg += QString("[%1,%2,%3]")
                .arg(it->value("player1_username").toString(),
                     it->value("player2_username").toString(),
                     it->value("winner_username").toString());
            }
            historyMsg += "\n";
            for (auto client : clients.keys()) {
                if (clients[client] == username) {
                    client->write(historyMsg.toUtf8());
                    client->flush();
                    break;
                }
            }
        }
    }

    QString message = QString("G[END][%1]").arg(winner);
    message += "\n";
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
    qDebug() << "Deck reset for new round with" << currentDeck.size() << "cards";
    CardManager::Shuffle(currentDeck);
    player1Cards.clear();
    player2Cards.clear();
    communityCards.clear();
    currentTurn = 0;
}

void Game::onCardViewTimeout()
{
    cardViewTimer->stop();
    startRound();
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

    if (currentRound == 1) {
        waitingForPlayer1 = (startingPlayerIndex == 0);
        waitingForPlayer2 = (startingPlayerIndex == 1);
    } else if (currentRound == 2) {
        waitingForPlayer1 = (startingPlayerIndex == 1);
        waitingForPlayer2 = (startingPlayerIndex == 0);
    } else if (currentRound == 3) {
        waitingForPlayer1 = (startingPlayerIndex == 0);
        waitingForPlayer2 = (startingPlayerIndex == 1);
    }
    qDebug() << "Round" << currentRound << "starting with:" << (waitingForPlayer1 ? playerUsernames[0] : playerUsernames[1]);

    currentDeck = CardManager::MakeDeck();
    qDebug() << "New deck created for round" << currentRound << "with" << currentDeck.size() << "cards";
    CardManager::Shuffle(currentDeck);
    sendCommunityCards();
}

void Game::handleClientMessage()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (!client) {
        qDebug() << "No valid client sender";
        return;
    }

    while (client->canReadLine()) {
        QByteArray line = client->readLine().trimmed();
        QString message = QString::fromUtf8(line);
        qDebug() << "Received message from" << clients[client] << ":" << message;

        // Parse the message
        if (message.startsWith("G[")) {
            int firstEnd = message.indexOf(']');
            if (firstEnd != -1) {
                QString command = message.mid(2, firstEnd - 2); // Extract command, e.g., "CHAT" or "CC"
                QString rest = message.mid(firstEnd + 1).trimmed();

                if (command == "CHAT") {
                    // Assuming format: G[CHAT][sender][message]
                    int senderEnd = rest.indexOf(']');
                    if (senderEnd != -1) {
                        QString sender = rest.mid(1, senderEnd - 1); // Extract sender
                        QString chatMessage = rest.mid(senderEnd + 2).trimmed(); // Extract message
                        processChatMessage(client, sender, chatMessage);
                    }
                } else if (command == "CC") {
                    // Assuming format: G[CC][suit-rank]
                    QString suitRank = rest.mid(1, rest.indexOf(']') - 1);
                    processPlayerCardChoice(client, suitRank);
                } else {
                    qDebug() << "Unknown command from" << clients[client] << ":" << command;
                }
            }
        } else {
            qDebug() << "Invalid message format from" << clients[client] << ":" << message;
        }
    }
}
