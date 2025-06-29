#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include <QMessageBox>
#include "client.h"
#include "InputValidator.h"
#include "player.h"

namespace Ui {
class Login;
}

class Login : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor
    explicit Login(Client* client, QWidget *parent = nullptr);
    // Destructor
    ~Login();

private slots:
    // Slot for login button click
    void onLoginButtonClicked();

private:
    Ui::Login *ui;
    Client* client;
};

#endif // LOGIN_H