#include "login.h"
#include "ui_login.h"
#include "client.h"
#include <QMessageBox>

Login::Login(Player& player, Client *client, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::Login),
    player(player),
    client(client)
{
    ui->setupUi(this);
     ui->Login_Btn->setEnabled(false);
    connect(ui->Log_Email_Radio,&QRadioButton::clicked,this,&Login::Update_Login_Btn);
    connect(ui->Log_Uname_Radio,&QRadioButton::clicked,this,&Login::Update_Login_Btn);
    connect(ui->UE_text,&QTextEdit::textChanged,this,&Login::Update_Login_Btn);
    connect(ui->Pwd_text,&QTextEdit::textChanged,this,&Login::Update_Login_Btn);
}

Login::~Login()
{
    delete ui;
}



void Login::Update_Login_Btn()
{
    bool RadioCheck = ui->Log_Email_Radio->isChecked() || ui->Log_Uname_Radio->isChecked();
    bool Textcheck = !ui->UE_text->toPlainText().isEmpty() && !ui->Pwd_text->toPlainText().isEmpty();

    if(RadioCheck && Textcheck) {
        ui->Login_Btn->setEnabled(true);
    }
    else {
        ui->Login_Btn->setEnabled(false);
    }
}

void Login::on_Login_Btn_clicked()
{
    //۱.بررسی کن کاربر لاگین با یوزرنیم را انتخاب کرده یا لاگین با ایمیل
    //۲.اگر لاگین با ایمیل بود بررسی کن که ایا ایمیل معتبر است
    //۳.بررسی کن ایا رمز معتبر است
    //۴.اگر همه چیز برای ارسال اوکی بود  متود  زیر را انجام بده
    //client->WriteToServer(QString data)
    // دیتا باید یک کیو استرینگ باشد و در قالب زیر
    // "L[type][Username or email][password]"
    // if user choosed login with email then type should be "E" if it is Username then "U"

}

