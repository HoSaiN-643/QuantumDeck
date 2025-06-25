#ifndef LOGINORSIGNUP_H
#define LOGINORSIGNUP_H

#include <QMainWindow>

namespace Ui {
class LoginOrSignUp;
}

class LoginOrSignUp : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginOrSignUp(QWidget *parent = nullptr);
    ~LoginOrSignUp();

private:
    Ui::LoginOrSignUp *ui;
};

#endif // LOGINORSIGNUP_H
