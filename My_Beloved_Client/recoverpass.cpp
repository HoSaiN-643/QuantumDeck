#include "recoverpass.h"
#include "ui_recoverpass.h"
#include <QMessageBox>

RecoverPass::RecoverPass(Client* client, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RecoverPass)
    , client(client)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &RecoverPass::on_pushButton_clicked);
}

RecoverPass::~RecoverPass()
{
    delete ui;
}

void RecoverPass::on_pushButton_clicked()
{
    QString phone = ui->textEdit->toPlainText().simplified();
    if (phone.isEmpty() || phone.length() < 10) {
        QMessageBox::warning(this, "شماره تلفن نامعتبر", "لطفاً یک شماره تلفن معتبر وارد کنید.");
    } else {
        client->WriteToServer("R[" + phone + "]");
        QMessageBox::information(this, "درخواست ارسال شد", "درخواست بازیابی رمز عبور ارسال شد.");
    }
}