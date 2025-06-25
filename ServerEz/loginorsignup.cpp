#include "loginorsignup.h"
#include "ui_loginorsignup.h"

LoginOrSignUp::LoginOrSignUp(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginOrSignUp)
{
    ui->setupUi(this);
}

LoginOrSignUp::~LoginOrSignUp()
{
    delete ui;
}
