#include "recoverpass.h"
#include "ui_recoverpass.h"
#include "Client.h"

RecoverPass::RecoverPass(Client* client,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RecoverPass)
{
    ui->setupUi(this);

    //شماره ای که کاربر در یو ای وارد شده را چک کن که معتبر هست یا نه اگر نبود یک ارور در صفحه نمایش بده که شمارت معتبر نیسیت
    //اگر معتبر بود با متود زیر یک پیام به سرور ارسال شود
    //client.WriteToServer(QString data)
    // در فرمت زیر
    // "R[phone]"
}

RecoverPass::~RecoverPass()
{
    delete ui;
}
