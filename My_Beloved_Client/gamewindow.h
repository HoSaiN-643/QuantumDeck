#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include "client.h"

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GameWindow(Client* client, QWidget *parent = nullptr);
    ~GameWindow();

    void displayDiamondCards(const QString& yourCard, const QString& opponentCard); // متد جدید
    void displayCommunityCards(const QStringList& cards);
    void displayWaitingMessage(const QString& message);
    void displayGameStatus(const QString& status);
    void displayChatMessage(const QString& message);
    void clearCards(); // به public تغییر

private slots:
    void onSendButtonClicked();
    void handleServerMessage();
    void onDiamondCardTimeout();
    void onCardSelection(int index);

private:
    Client* m_client;
    QTimer* m_diamondTimer;
    QVector<QPushButton*> m_cardButtons;
    QLabel* m_diamondLabel;
    QLabel* m_statusLabel;
    QTextEdit* m_chatBox;
    QLineEdit* m_messageInput;
    QPushButton* m_sendButton;
    QVBoxLayout* m_mainLayout;
    QString m_yourDiamondCard;
    QString m_opponentDiamondCard;
    bool m_isYourTurn;
    int m_remainingCards;

    void setupUI();
};

#endif // GAMEWINDOW_H
