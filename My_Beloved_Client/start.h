#ifndef START_H
#define START_H

#include <QObject>
#include <QTimer>

// فقط اعلام وجود کلاس‌ها
class Client;
class Connect;

class Start : public QObject
{
    Q_OBJECT

    Client *c;
    Connect *window;


public:
    explicit Start(QObject *parent = nullptr);
    ~Start();

private slots:

};

#endif // START_H
