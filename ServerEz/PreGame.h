#ifndef PREGAME_H
#define PREGAME_H

#include <QObject>
#include <QTcpSocket>
#include <QVector>
#include <QPair>
#include <QStringList>

class PreGame : public QObject
{
    Q_OBJECT

public:
    bool IsServerFull{false};

    // ctor: count = تعداد بازیکنِ مورد نظر (مثلاً 2)، p = جفت (socket, username) اولین بازیکن
    explicit PreGame(int count,
                     QPair<QTcpSocket*,QString> p,
                     QObject *parent = nullptr);

    // اضافه کردن بازیکن جدید
    void AddPlayer(QPair<QTcpSocket*,QString> p);

private:
    int PlayerCnt{0};           // مثلاً 2
    int WaitingPlayers{0};      // تاکنون چند نفر اضافه شدند
    QVector<QPair<QTcpSocket*,QString>> Players;

    // می‌فرستد P[count][waiting][Searching for a match] به همهٔ Players
    void sendSearching();

    // می‌فرستد P[count][Found][<نام بقیه>...] به همهٔ Players
    void sendFound();
};

#endif // PREGAME_H
