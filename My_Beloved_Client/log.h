#ifndef LOG_H
#define LOG_H

#include <QMainWindow>
#include <login.h>
#include <signup.h>


namespace Ui {
class Log;
}

class Log : public QMainWindow
{
    Q_OBJECT
    Login l;
    Signup s;

public:
    explicit Log(QWidget *parent = nullptr);
    ~Log();
    void Show();

private slots:
    void on_Login_Btn_clicked();

    void on_Signup_btn_clicked();

private:
    Ui::Log *ui;
};

#endif // LOG_H
