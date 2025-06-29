#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include "mainmenu.h"
namespace Ui { class Login; }
class Client;
class MainMenu;
class Player;

class Login : public QMainWindow
{
    Q_OBJECT

public:
    explicit Login(Player& player, Client *client, QWidget *parent = nullptr);
    ~Login();

private slots:
    void Update_Login_Btn();
    void Login_Btn_Clicked();
    void On_Succesful_Login();

private:
    Ui::Login *ui;
    Client     *client;
    Player&    player;
    MainMenu*  menuWindow;
};

#endif // LOGIN_H
