#include "login.h"
#include "ui_login.h"

Login::Login(Client* client, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Login)
    , client(client)
{
    ui->setupUi(this);

    // اتصال دکمه ورود
    connect(ui->login_button, &QPushButton::clicked, this, &Login::onLoginButtonClicked);
}

Login::~Login()
{
    delete ui;
}

void Login::onLoginButtonClicked()
{
    QString username = ui->username_lineEdit->text().trimmed();
    QString password = ui->password_lineEdit->text();

    // بررسی خالی نبودن فیلدها
    if (username.isEmpty()) {
        QMessageBox::warning(this, "خطا", "نام کاربری نمی‌تواند خالی باشد.");
        return;
    }
    if (password.isEmpty()) {
        QMessageBox::warning(this, "خطا", "رمز عبور نمی‌تواند خالی باشد.");
        return;
    }

    // اعتبارسنجی نام کاربری (حداقل ۵ کاراکتر)
    if (username.length() < 5) {
        QMessageBox::warning(this, "خطا", "نام کاربری باید حداقل ۵ کاراکتر باشد.");
        return;
    }

    // اعتبارسنجی رمز عبور
    QString error = InputValidator::validatePassword(password);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "خطا", "رمز عبور: " + error);
        return;
    }

    // ارسال اطلاعات به سرور
    QString message = QString("L[LOGIN][%1][%2]").arg(username).arg(password);
    client->WriteToServer(message);

    // اطلاع‌رسانی موفقیت (فرض بر موفقیت)
    QMessageBox::information(this, "موفقیت", "ورود با موفقیت انجام شد.");
}