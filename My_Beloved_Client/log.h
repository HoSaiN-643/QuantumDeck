#ifndef LOG_H
#define LOG_H

#include <QMainWindow>
#include "player.h"

namespace Ui { class Log; }

// Forward declarations
class Client;
class Login;
class Signup;

class Log : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor
    explicit Log(Player& player, Client *client, QWidget *parent = nullptr);
    // Destructor
    ~Log();

    // Show the window
    void Show();

private slots:
    // Slot for login button click
    void on_Login_Btn_clicked();
    // Slot for signup button click
    void on_Signup_btn_clicked();

private:
    Ui::Log *ui;    // UI pointer
    Client   *client; // Client pointer for network connection
    Login    *l;    // Login window
    Signup   *s;    // Signup window
};

#endif // LOG_H