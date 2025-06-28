#include "change_profile.h"
#include "ui_change_profile.h"

Change_profile::Change_profile(Player& player, Client* client, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Change_profile)
    , player(player)
    , client(client)
{
    ui->setupUi(this);

    // پر کردن فیلدها با اطلاعات بازیکن
    ui->phone_lineEdit->setText(player.m_phone);
    ui->firstname_lineEdit->setText(player.m_firstname);
    ui->lastname_lineEdit->setText(player.m_lastname);
    ui->email_lineEdit->setText(player.m_email);
    ui->username_lineEdit->setText(player.m_username);

    // خالی کردن فیلدهای رمز عبور
    ui->current_password_lineEdit->clear();
    ui->new_password_lineEdit->clear();
    ui->confirm_new_password_lineEdit->clear();

    // غیرفعال کردن دکمه ذخیره
    ui->save_button->setEnabled(false);

    // اتصال سیگنال‌های textChanged
    connect(ui->phone_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->firstname_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->lastname_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->email_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->username_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->current_password_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->new_password_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->confirm_new_password_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);

    // اتصال دکمه ذخیره
    connect(ui->save_button, &QPushButton::clicked, this, &Change_profile::onSaveButtonClicked);
}

Change_profile::~Change_profile()
{
    delete ui;
}

void Change_profile::onFieldChanged()
{
    ui->save_button->setEnabled(true);
}

void Change_profile::onSaveButtonClicked()
{
    // دریافت مقادیر فیلدها
    QString phone = ui->phone_lineEdit->text().trimmed();
    QString firstname = ui->firstname_lineEdit->text().trimmed();
    QString lastname = ui->lastname_lineEdit->text().trimmed();
    QString email = ui->email_lineEdit->text().trimmed();
    QString username = ui->username_lineEdit->text().trimmed();

    // بررسی خالی نبودن فیلدها
    if (phone.isEmpty()) {
        QMessageBox::warning(this, "خطا", "تلفن نمی‌تواند خالی باشد.");
        return;
    }
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
    if (username.isEmpty()) {
        QMessageBox::warning(this, "خطا", "نام کاربری نمی‌تواند خالی باشد.");
        return;
    }

    // اعتبارسنجی نام و نام خانوادگی (حداقل ۲ کاراکتر)
    if (firstname.length() < 2) {
        QMessageBox::warning(this, "خطا", "نام باید حداقل ۲ کاراکتر باشد.");
        return;
    }
    if (lastname.length() < 2) {
        QMessageBox::warning(this, "خطا", "نام خانوادگی باید حداقل ۲ کاراکتر باشد.");
        return;
    }

    // اعتبارسنجی نام کاربری (حداقل ۵ کاراکتر)
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

    // مدیریت رمز عبور
    QString password_to_send = player.m_password;
    QString current_password = ui->current_password_lineEdit->text();
    QString new_password = ui->new_password_lineEdit->text();
    QString confirm_new_password = ui->confirm_new_password_lineEdit->text();

    if (!current_password.isEmpty()) {
        if (current_password != player.m_password) {
            QMessageBox::warning(this, "خطا", "رمز فعلی نادرست است.");
            return;
        }
        error = InputValidator::validatePassword(current_password);
        if (!error.isEmpty()) {
            QMessageBox::warning(this, "خطا", "رمز فعلی: " + error);
            return;
        }
        if (!new_password.isEmpty()) {
            if (new_password != confirm_new_password) {
                QMessageBox::warning(this, "خطا", "رمز جدید و تأیید آن مطابقت ندارند.");
                return;
            }
            error = InputValidator::validatePassword(new_password);
            if (!error.isEmpty()) {
                QMessageBox::warning(this, "خطا", "رمز جدید: " + error);
                return;
            }
            password_to_send = new_password;
        }
    }

    // ارسال اطلاعات به سرور
    QString message = QString("C[CF][%1][%2][%3][%4][%5][%6]")
                         .arg(firstname)
                         .arg(lastname)
                         .arg(email)
                         .arg(phone)
                         .arg(username)
                         .arg(password_to_send);
    client->WriteToServer(message);

    // اطلاع‌رسانی موفقیت
    QMessageBox::information(this, "موفقیت", "تغییرات با موفقیت ذخیره شد.");
}