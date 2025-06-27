#ifndef SIGNUP_H
#define SIGNUP_H

#include <QMainWindow>
namespace Ui { class Signup; }
class Client;

class Signup : public QMainWindow
{
    Q_OBJECT

public:
    explicit Signup(Client *client, QWidget *parent = nullptr);
    ~Signup();

private slots:
    void on_Register_Btn_clicked();

private:
    Ui::Signup *ui;
    Client      *client;
};

#endif // SIGNUP_H
