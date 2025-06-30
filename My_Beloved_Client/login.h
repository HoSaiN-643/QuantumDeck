#pragma once
#include <QMainWindow>
#include "player.h"
#include "client.h"

class MainMenu;

namespace Ui {
class Login;
}

class Login : public QMainWindow
{
    Q_OBJECT

public:
    explicit Login(Player& player, Client *client, QWidget *parent = nullptr);
    ~Login();

public slots:
    void Open_menu();

private slots:
    void Update_Login_Btn();
    void Login_Btn_Clicked();

private:
    Ui::Login *ui;
    Player& player;
    Client *client;
    MainMenu *menuWindow;
};
