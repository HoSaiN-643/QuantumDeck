#ifndef CONNECT_H
#define CONNECT_H

#include <QMainWindow>

namespace Ui { class connect; }

class Client;
class Log;
class Player;

class Connect : public QMainWindow
{
    Q_OBJECT

public:
    explicit Connect(Player& player, Client *client, QWidget *parent = nullptr);
    ~Connect();

    // Expose methods for direct calls
    void OnConnected();
    void OnErrorOccurred(const QString &errorString);

private slots:
    void Connect_Btn_clicked();

private:
    Ui::connect *ui;
    Client *client;
    Log *logWindow;
    Player &player;
};

#endif // CONNECT_H
