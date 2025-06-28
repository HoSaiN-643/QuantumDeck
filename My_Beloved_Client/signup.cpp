#include "signup.h"
#include "ui_signup.h"

Signup::Signup(Client* client, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Signup)
    , client(client)
{
    ui->setupUi(this);

    // اتصال دکمه ثبت‌نام
    connect(ui->signup_button, &QPushButton::clicked, this, &Signup::onSignupButtonClicked);
}

Signup::~Signup()
{
    delete ui;
}

void Signup::onSignupButtonClicked()
{
    QString firstname = ui->firstname_lineEdit->text().trimmed();
    QString lastname = ui->lastname_lineEdit->text().trimmed();
    QString email = ui->email_lineEdit->text().trimmed();
    QString phone = ui->phone_lineEdit->text().trimmed();
    QString username = ui->username_lineEdit->text().trimmed();
    QString password = ui->password_lineEdit->text();
    QString confirm_password = ui->confirm_password_lineEdit->text();

    // بررسی خالی نبودن فیلدها
    if (firstname.isEmpty()) {
        QMessageBox::warning(this, "خطا", "نام نمی‌تواند خالی باشد.");
        return;
    }
    if (lastname.isEmpty()) {
        QMessageBox::warning(this, "خطا", "نام خانوادگی نمی‌تواند خالی باشد.");
        return;
    }
    if (email.isEmpty()) {
        QMessageBox::warning(this, "خطا", "ایمیل نمی‌تواند خالی باشد.");
        return;
    }
    if (phone.isEmpty()) {
        QMessageBox::warning(this, "خطا", "تلفن نمی‌تواند خالی باشد.");
        return;
    }
    if (username.isEmpty()) {
        QMessageBox::warning(this, "خطا", "نام کاربری نمی‌تواند خالی باشد.");
        return;
    }
    if (password.isEmpty()) {
        QMessageBox::warning(this, "خطا", "رمز عبور نمی‌تواند خالی باشد.");
        return;
    }

    // اعتبارسنجی نام و نام خانوادگی
    if (firstname.length() < 2) {
        QMessageBox::warning(this, "خطا", "نام باید حداقل ۲ کاراکتر باشد.");
        return;
    }
    if (lastname.length() < 2) {
        QMessageBox::warning(this, "خطا", "نام خانوادگی باید حداقل ۲ کاراکتر باشد.");
        return;
    }

    // اعتبارسنجی نام کاربری
    if (username.length() < 5) {
        QMessageBox::warning(this, "خطا", "نام کاربری باید حداقل ۵ کاراکتر باشد.");
        return;
    }

    // اعتبارسنجی ایمیل و تلفن
    QString error = InputValidator::validateEmail(email);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "خطا", error);
        return;
    }
    error = InputValidator::validatePhone(phone);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "خطا", error);
        return;
    }

    // اعتبارسنجی رمز عبور
    error = InputValidator::validatePassword(password);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "خطا", "رمز عبور: " + error);
        return;
    }
    if (password != confirm_password) {
        QMessageBox::warning(this, "خطا", "رمز عبور و تأیید آن مطابقت ندارند.");
        return;
    }

    // ارسال اطلاعات به سرور
    QString message = QString("S[SIGNUP][%1][%2][%3][%4][%5][%6]")
                         .arg(firstname)
                         .arg(lastname)
                         .arg(email)
                         .arg(phone)
                         .arg(username)
                         .arg(password);
    client->WriteToServer(message);

    // اطلاع‌رسانی موفقیت
    QMessageBox::information(this, "موفقیت", "ثبت‌نام با موفقیت انجام شد.");
}