#include "log.h"
#include "ui_log.h"
#include "login.h"
#include "signup.h"

Log::Log(Client *client, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::Log),
    client(client),
    l(new Login(client, this)),
    s(new Signup(client, this))
{
    ui->setupUi(this);
    connect(ui->Login_Btn,&QPushButton::clicked,this,&Log::on_Login_Btn_clicked);
    // connect(ui->Signup_btn,&QPushButton::clicked,this,&Log::on_Signup_btn_clicked);
}

Log::~Log()
{
    delete ui;
    // l و s به عنوان child پاک می‌شوند
}

void Log::Show()
{
    this->show();   // قبلاً اشتباه اینجا فراخوانی Show می‌شد که recursion ایجاد می‌کرد
}

void Log::on_Login_Btn_clicked()
{
      this->close();
    l->show();


}

void Log::on_Signup_btn_clicked()
{
    this->close();
    s->show();


}
