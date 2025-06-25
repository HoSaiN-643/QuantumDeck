#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>

namespace Ui {
class Login;
}

class Login : public QMainWindow
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    void on_Log_Email_Radio_clicked();

    void on_Log_Uname_Btn_clicked();

    void updateLoginBtn();

private:
    Ui::Login *ui;
};

#endif // LOGIN_H
