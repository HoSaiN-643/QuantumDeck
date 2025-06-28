#include "recoverpass.h"
#include "ui_recoverpass.h"

Recoverpass::Recoverpass(Client* client, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Recoverpass)
    , client(client)
{
    ui->setupUi(this);

    // اتصال دکمه بازیابی
    connect(ui->recover_button, &QPushButton::clicked, this, &Recoverpass::onRecoverButtonClicked);
}

Recoverpass::~Recoverpass()
{
    delete ui;
}

void Recoverpass::onRecoverButtonClicked()
{
    QString email = ui->email_lineEdit->text().trimmed();

    // بررسی خالی نبودن ایمیل
    if (email.isEmpty()) {
        QMessageBox::warning(this, "خطا", "ایمیل نمی‌تواند خالی باشد.");
        return;
    }

    // اعتبارسنجی ایمیل
    QString error = InputValidator::validateEmail(email);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "خطا", error);
        return;
    }

    // ارسال درخواست بازیابی به سرور
    QString message = QString("R[RECOVER][%1]").arg(email);
    client->WriteToServer(message);

    // اطلاع‌رسانی موفقیت
    QMessageBox::information(this, "موفقیت", "درخواست بازیابی رمز عبور ارسال شد.");
}