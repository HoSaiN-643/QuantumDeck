#include "gamewindow.h"
#include "client.h"
#include <QDebug>

GameWindow::GameWindow(Client* client, QWidget *parent)
    : QMainWindow(parent)
    , m_client(client)
    , m_diamondTimer(new QTimer(this))
    , m_isYourTurn(false)
    , m_remainingCards(0)
{
    setupUI();

    connect(m_client->getSocket(), &QTcpSocket::readyRead, this, &GameWindow::handleServerMessage);
    connect(m_diamondTimer, &QTimer::timeout, this, &GameWindow::onDiamondCardTimeout);
    connect(m_sendButton, &QPushButton::clicked, this, &GameWindow::onSendButtonClicked);

    displayGameStatus("Waiting for game to start...");
}

GameWindow::~GameWindow()
{
    delete m_diamondTimer;
    for (QPushButton* button : m_cardButtons) {
        delete button;
    }
    delete m_diamondLabel;
    delete m_statusLabel;
    delete m_chatBox;
    delete m_messageInput;
    delete m_sendButton;
    delete m_mainLayout;
}

void GameWindow::setupUI()
{
    QWidget* centralWidget = new QWidget(this);
    m_mainLayout = new QVBoxLayout(centralWidget);

    m_diamondLabel = new QLabel("No Diamond cards yet", this);
    m_statusLabel = new QLabel("Waiting...", this);
    m_chatBox = new QTextEdit(this);
    m_chatBox->setReadOnly(true);
    m_messageInput = new QLineEdit(this);
    m_sendButton = new QPushButton("Send", this);

    m_mainLayout->addWidget(m_diamondLabel);
    m_mainLayout->addWidget(m_statusLabel);
    m_mainLayout->addWidget(m_chatBox);
    m_mainLayout->addWidget(m_messageInput);
    m_mainLayout->addWidget(m_sendButton);

    for (int i = 0; i < 7; ++i) {
        QPushButton* button = new QPushButton(this);
        button->setVisible(false);
        connect(button, &QPushButton::clicked, this, [this, i]() { onCardSelection(i); });
        m_cardButtons.append(button);
        m_mainLayout->addWidget(button);
    }

    setCentralWidget(centralWidget);
}

void GameWindow::onSendButtonClicked()
{
    QString message = m_messageInput->text().trimmed();
    if (!message.isEmpty()) {
        qDebug() << "Sending chat message content:" << message;
        m_client->sendChatMessage(message + "\n");
        m_messageInput->clear();
    } else {
        qDebug() << "Empty message, not sending.";
    }
}

void GameWindow::handleServerMessage()
{
    while (m_client->getSocket()->bytesAvailable()) {
        QString message = m_client->getSocket()->readLine().trimmed();
        qDebug() << "Client received:" << message;

        QChar cmd = message.at(0);
        QString payload = message.mid(1);
        QStringList fields = m_client->extractFields(payload);
        qDebug() << "Type received:" << cmd << ", Fields:" << fields;

        if (fields.isEmpty()) {
            qDebug() << "Empty fields received";
            continue;
        }

        switch (cmd.unicode()) {
        case 'G':
            if (fields[0] == "ST") {
                if (fields.size() >= 4) {
                    qDebug() << "Handling game message with fields:" << fields;
                    displayDiamondCards(fields[2], fields[4]);
                }
            } else if (fields[0] == "CC") {
                if (fields[1] == "YOU") {
                    m_isYourTurn = true;
                    QStringList cards = fields.mid(2);
                    displayCommunityCards(cards);
                    m_remainingCards = cards.size();
                } else if (fields[1] == "WAIT") {
                    m_isYourTurn = false;
                    displayWaitingMessage("Waiting for opponent to choose...");
                }
            } else if (fields[0] == "RF") {
                if (fields.size() >= 2) {
                    displayGameStatus(QString("Round winner: %1").arg(fields[1]));
                    clearCards();
                }
            } else if (fields[0] == "END") {
                if (fields.size() >= 2) {
                    displayGameStatus(QString("Game ended. Winner: %1").arg(fields[1]));
                    clearCards();
                    close();
                }
            } else if (fields[0] == "CHAT") {
                if (fields.size() >= 3) {
                    displayChatMessage(QString("%1: %2").arg(fields[1], fields[2]));
                }
            }
            break;
        }
    }
}

void GameWindow::displayDiamondCards(const QString& yourCard, const QString& opponentCard)
{
    m_yourDiamondCard = yourCard;
    m_opponentDiamondCard = opponentCard;
    m_diamondLabel->setText(QString("Your Diamond: %1, Opponent's Diamond: %2")
                                .arg(m_yourDiamondCard, m_opponentDiamondCard));
    displayGameStatus("View your Diamond card for 5 seconds...");
    m_diamondTimer->start(5000);
}

void GameWindow::displayCommunityCards(const QStringList& cards)
{
    clearCards();
    for (int i = 0; i < cards.size() && i < 7; ++i) {
        m_cardButtons[i]->setText(cards[i]);
        m_cardButtons[i]->setVisible(true);
    }
    displayGameStatus("Select a card to play...");
}

void GameWindow::displayWaitingMessage(const QString& message)
{
    displayGameStatus(message);
}

void GameWindow::displayGameStatus(const QString& status)
{
    m_statusLabel->setText(status);
}

void GameWindow::displayChatMessage(const QString& message)
{
    m_chatBox->append(message);
}

void GameWindow::onDiamondCardTimeout()
{
    m_diamondLabel->clear();
    displayGameStatus("Selecting community cards...");
    m_client->WriteToServer("G[READY]\n");
}

void GameWindow::onCardSelection(int index)
{
    if (m_isYourTurn && m_cardButtons[index]->isVisible()) {
        QString selectedCard = m_cardButtons[index]->text();
        m_client->sendCardSelection(selectedCard);
        m_cardButtons[index]->setVisible(false);
        m_remainingCards--;
        displayGameStatus("Card selected, waiting for opponent...");
        m_isYourTurn = false;
    }
}

void GameWindow::clearCards()
{
    for (QPushButton* button : m_cardButtons) {
        button->setVisible(false);
    }
    m_remainingCards = 0;
}
