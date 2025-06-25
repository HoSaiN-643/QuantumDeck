#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
    ui->Login_Btn->setEnabled(false);
    connect(ui->Log_Email_Radio,&QRadioButton::clicked,this,&Login::updateLoginBtn);
    connect(ui->Log_Uname_Radio,&QRadioButton::clicked,this,&Login::updateLoginBtn);
    connect(ui->Confirm_Pwd_text,&QTextEdit::textChanged,this,&Login::updateLoginBtn);
    connect(ui->Pwd_text,&QTextEdit::textChanged,this,&Login::updateLoginBtn);
    connect(ui->UE_text,&QTextEdit::textChanged,this,&Login::updateLoginBtn);
}

Login::~Login()
{
    delete ui;
}

void Login::on_Log_Email_Radio_clicked()
{
    ui->UE_Label_2->setText("Email");

}


void Login::on_Log_Uname_Btn_clicked()
{
    ui->UE_Label_2->setText("Username");
}

void Login::updateLoginBtn()
{
    bool RadioCheck = ui->Log_Email_Radio->isChecked() || ui->Log_Uname_Radio->isChecked();
    bool TextCheck = !ui->Pwd_text->toPlainText().isEmpty() && !ui->UE_text->toPlainText().isEmpty()
                     && !ui->Confirm_Pwd_text->toPlainText().isEmpty();

    ui->Login_Btn->setEnabled(RadioCheck && TextCheck);
}


