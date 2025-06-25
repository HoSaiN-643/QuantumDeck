#include "log.h"
#include "ui_log.h"

Log::Log(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Log)
{
    ui->setupUi(this);
}

Log::~Log()
{
    delete ui;
}

void Log::Show()
{
    this->Show();
}



void Log::on_Login_Btn_clicked()
{
    l.show();
    this->close();
}


void Log::on_Signup_btn_clicked()
{
    s.show();
    this->close();
}

