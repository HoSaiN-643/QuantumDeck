#ifndef SIGNUP_H
#define SIGNUP_H

#include <QMainWindow>
#include <QMessageBox>
#include "client.h"
#include "InputValidator.h"
#include "player.h"

namespace Ui {
class Signup;
}

class Signup : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor
    explicit Signup(Client* client, QWidget *parent = nullptr);
    // Destructor
    ~Signup();

private slots:
    // Slot for signup button click
    void onSignupButtonClicked();

private:
    Ui::Signup *ui;
    Client* client;
};

#endif // SIGNUP_H