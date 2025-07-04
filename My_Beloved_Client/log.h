#pragma once
#include <QMainWindow>

namespace Ui { class Log; }

class Client;
class Login;
class Signup;
class Player;

class Log : public QMainWindow
{
    Q_OBJECT

public:
    explicit Log(Player& player, Client *client, QWidget *parent = nullptr);
    ~Log();

    void Show();

private slots:
    void Login_Btn_clicked();
    void Signup_btn_clicked();

private:
    Ui::Log *ui;
    Client *client;
    Login *l;
    Signup *s;
    Player &player;
};
