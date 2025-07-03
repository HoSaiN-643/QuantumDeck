#pragma once
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
