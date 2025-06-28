#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
namespace Ui { class Login; }
class Client;
class Mainmenu;

class Login : public QMainWindow
{
    Q_OBJECT

public:
    explicit Login(Client *client, QWidget *parent = nullptr);
    ~Login();

private slots:

    void Update_Login_Btn();

    void on_Login_Btn_clicked();

private:
    Ui::Login *ui;
    Client     *client;
    Mainmenu* menuWindow;

};

#endif // LOGIN_H
