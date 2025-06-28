#ifndef SIGNUP_H
#define SIGNUP_H

#include <QMainWindow>
#include <QMessageBox>
#include "client.h"
#include "InputValidator.h"

namespace Ui {
class Signup;
}

class Signup : public QMainWindow
{
    Q_OBJECT

public:
    explicit Signup(Client* client, QWidget *parent = nullptr);
    ~Signup();

private slots:
    void onSignupButtonClicked();

private:
    Ui::Signup *ui;
    Client* client;
};

#endif // SIGNUP_H