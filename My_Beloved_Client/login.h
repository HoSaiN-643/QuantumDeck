#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include <QMessageBox>
#include "client.h"
#include "InputValidator.h"

namespace Ui {
class Login;
}

class Login : public QMainWindow
{
    Q_OBJECT

public:
    explicit Login(Client* client, QWidget *parent = nullptr);
    ~Login();

private slots:
    void onLoginButtonClicked();

private:
    Ui::Login *ui;
    Client* client;
};

#endif // LOGIN_H