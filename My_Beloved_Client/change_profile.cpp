#include "change_profile.h"
#include "ui_change_profile.h"
#include "InputValidator.h"
#include <QMessageBox>

Change_profile::Change_profile(Player& player, Client* client, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::Change_profile),
    player(player),
    client(client)
{
    if (!client) {
        QMessageBox::critical(this, "Error", "No connection to the server.");
        close();
        return;
    }

    ui->setupUi(this);

    // Store original values for comparison
    original_values.phone = player.phone();
    original_values.firstname = player.firstName();
    original_values.lastname = player.lastName();
    original_values.email = player.email();
    original_values.username = player.username();

    // Populate fields with player data (except password fields)
    ui->phone_lineEdit->setText(player.phone());
    ui->firstname_lineEdit->setText(player.firstName());
    ui->lastname_lineEdit->setText(player.lastName());
    ui->email_lineEdit->setText(player.email());
    ui->username_lineEdit->setText(player.username());

    // Clear password fields
    ui->current_password_lineEdit->clear();
    ui->new_password_lineEdit->clear();
    ui->confirm_new_password_lineEdit->clear();

    // Disable save button initially
    ui->save_button->setEnabled(false);

    // Connect textChanged signals to detect changes
    connect(ui->phone_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->firstname_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->lastname_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->email_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->username_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->current_password_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->new_password_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->confirm_new_password_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);

    // Connect save button
    connect(ui->save_button, &QPushButton::clicked, this, &Change_profile::onSaveButtonClicked);
}

Change_profile::~Change_profile()
{
    delete ui;
}

void Change_profile::onFieldChanged()
{
    // Check if any field has changed
    bool has_changes =
        ui->phone_lineEdit->text().trimmed() != original_values.phone ||
        ui->firstname_lineEdit->text().trimmed() != original_values.firstname ||
        ui->lastname_lineEdit->text().trimmed() != original_values.lastname ||
        ui->email_lineEdit->text().trimmed() != original_values.email ||
        ui->username_lineEdit->text().trimmed() != original_values.username ||
        !ui->current_password_lineEdit->text().isEmpty() ||
        !ui->new_password_lineEdit->text().isEmpty() ||
        !ui->confirm_new_password_lineEdit->text().isEmpty();

    ui->save_button->setEnabled(has_changes);
}

void Change_profile::onSaveButtonClicked()
{
    // غیرفعال کردن دکمه ذخیره تا زمانی که پردازش کامل شود
    ui->save_button->setEnabled(false);

    // دریافت مقادیر از فیلدهای ورودی
    QString phone = ui->phone_lineEdit->text().trimmed();
    QString firstname = ui->firstname_lineEdit->text().trimmed();
    QString lastname = ui->lastname_lineEdit->text().trimmed();
    QString email = ui->email_lineEdit->text().trimmed();
    QString username = ui->username_lineEdit->text().trimmed();
    QString current_password = ui->current_password_lineEdit->text();
    QString new_password = ui->new_password_lineEdit->text();
    QString confirm_new_password = ui->confirm_new_password_lineEdit->text();

    // لیست خطاها برای اعتبارسنجی
    QStringList errors;

    // اعتبارسنجی فیلدهای اصلی با استفاده از InputValidator
    if (QString error = InputValidator::validateName(firstname, "First name"); !error.isEmpty())
        errors << error;
    if (QString error = InputValidator::validateName(lastname, "Last name"); !error.isEmpty())
        errors << error;
    if (QString error = InputValidator::validateEmail(email); !error.isEmpty())
        errors << error;
    if (QString error = InputValidator::validatePhone(phone); !error.isEmpty())
        errors << error;
    if (QString error = InputValidator::validateUsername(username); !error.isEmpty())
        errors << error;

    // مدیریت تغییر رمز عبور
    QString password_to_send;
    bool password_change_attempted = !current_password.isEmpty() && !new_password.isEmpty() && !confirm_new_password.isEmpty();

    if (password_change_attempted) {
        // کاربر قصد تغییر رمز عبور دارد
        if (current_password != player.password()) {
            errors << "Current password is incorrect.";
        }
        if (new_password != confirm_new_password) {
            errors << "New password and confirmation do not match.";
        }
        if (QString error = InputValidator::validatePassword(new_password); !error.isEmpty()) {
            errors << error;
        }
        if (errors.isEmpty()) {
            // اگر همه چیز درست بود، رمز جدید را برای ارسال تنظیم می‌کنیم
            password_to_send = new_password;
        }
    } else {
        // کاربر قصد تغییر رمز عبور ندارد یا فیلدها کامل پر نشده‌اند
        password_to_send = player.password();
    }

    // نمایش خطاها در صورت وجود
    if (!errors.isEmpty()) {
        QString errorText = "Please fix the following errors:\n\n";
        errorText += errors.join("\n");
        QMessageBox::warning(this, "Invalid Input", errorText);
        ui->save_button->setEnabled(true);
        return;
    }

    // ساخت پیام برای سرور با ۸ فیلد
    QString message = QString("C[CF][%1][%2][%3][%4][%5][%6][%7]")
                          .arg(firstname, lastname, email, phone, username, password_to_send, original_values.username);

    // ارسال پیام به سرور
    client->WriteToServer(message);
}



