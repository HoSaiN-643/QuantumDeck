#ifndef SIGNUP_H
#define SIGNUP_H

#include <QMainWindow>

namespace Ui { class Signup; }
class Client;
class Login;
class Player;

class Signup : public QMainWindow
{
    Q_OBJECT

public:
    explicit Signup(Player& player, Client *client, QWidget *parent = nullptr);
    ~Signup();

    void OnSuccesfullSignUp(); // Expose for direct call

private slots:
    void SignUp_Btn_clicked();
    void validateFields();

private:
    Ui::Signup *ui;
    Client *client;
    Login *loginWindow;
};

#endif // SIGNUP_H
