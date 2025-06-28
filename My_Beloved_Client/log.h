#ifndef LOG_H
#define LOG_H

#include <QMainWindow>
#include "player.h"

namespace Ui { class Log; }

// پیش‌اظهاری کلاس‌ها
class Client;
class Login;
class Signup;

class Log : public QMainWindow
{
    Q_OBJECT

public:
    explicit Log(Player& player, Client *client, QWidget *parent = nullptr);
    ~Log();

    void Show();

private slots:
    void on_Login_Btn_clicked();
    void on_Signup_btn_clicked();

private:
    Ui::Log *ui;
    Client   *client;
    Login    *l;
    Signup   *s;
};

#endif // LOG_H