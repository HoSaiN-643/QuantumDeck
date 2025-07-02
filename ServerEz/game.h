#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QTcpSocket>
#include <QVector>
#include <QMap>
#include <QTimer>
#include "cardmanager.h"

class Game : public QObject
{
    Q_OBJECT

public:
    explicit Game(const QStringList &players, QMap<QTcpSocket*, QString> &clients, QObject *parent = nullptr);
    void start();

private:
    QStringList playerUsernames; // نام کاربری بازیکنان
    QMap<QTcpSocket*, QString> &clients; // ارجاع به نقشه کلاینت‌ها
    QVector<Card> player1Cards; // کارت‌های بازیکن اول
    QVector<Card> player2Cards; // کارت‌های بازیکن دوم
    int player1Wins; // تعداد برد بازیکن اول
    int player2Wins; // تعداد برد بازیکن دوم
    int currentRound; // راند فعلی
    QVector<Card> currentDeck; // دسته کارت فعلی
    QVector<Card> communityCards; // 7 کارت مشترک
    QTimer *cardViewTimer; // تایمر 5 ثانیه برای نمایش کارت Diamond
    int startingPlayerIndex; // اندیس بازیکن شروع‌کننده (0 یا 1)
    bool waitingForPlayer1; // آیا منتظر انتخاب کارت بازیکن اول هستیم؟
    bool waitingForPlayer2; // آیا منتظر انتخاب کارت بازیکن دوم هستیم؟
    int currentTurn; // شماره نوبت فعلی در راند

    void startRound();
    void sendDiamondCards();
    void sendCommunityCards();
    void processPlayerCardChoice(QTcpSocket *client, const QStringList &fields);
    void determineRoundWinner();
    void checkGameWinner();
    void resetRound();

signals:
    void gameFinished(); // سیگنال برای پایان بازی

private slots:
    void onCardViewTimeout();
    void handleClientMessage(QTcpSocket *client, const QStringList &fields);
};

#endif // GAME_H
