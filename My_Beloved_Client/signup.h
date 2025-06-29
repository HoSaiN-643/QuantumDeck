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
    explicit Signup(Player& player,Client *client, QWidget *parent = nullptr);
    ~Signup();

private slots:
    void SignUp_Btn_clicked();  // Called when user clicks the sign up button
    void validateFields();
    void OnSuccesfullSignUp();    // Called on every field change to enable/disable the button

private:
    Ui::Signup *ui;
    Client     *client;
    Login* loginWindow;

};

#endif // SIGNUP_H
