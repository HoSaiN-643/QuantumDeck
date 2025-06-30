#pragma once
#include <QMainWindow>
#include "player.h"
#include "client.h"

class Login; // Forward declaration

namespace Ui {
class Signup;
}

class Signup : public QMainWindow
{
    Q_OBJECT

public:
    explicit Signup(Player& player, Client *client, QWidget *parent = nullptr);
    ~Signup();

public slots:
    void OnSuccesfullSignUp();

private slots:
    void SignUp_Btn_clicked();
    void validateFields();

private:
    Ui::Signup *ui;
    Client *client;
    Login *loginWindow;
};
