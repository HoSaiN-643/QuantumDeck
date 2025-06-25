#include "signup.h"
#include "ui_signup.h"

Signup::Signup(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Signup)
{
    ui->setupUi(this);
}

Signup::~Signup()
{
    delete ui;
}
