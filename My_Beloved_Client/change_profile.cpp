#include "change_profile.h"
#include "ui_change_profile.h"
#include "InputValidator.h"
#include <QMessageBox>
#include <QVector>
#include "client.h"
#include <QDebug>

Change_profile::Change_profile(Player& player, Client *client, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::Change_profile),
    player(player),
    client(client)
{
    ui->setupUi(this);
    ui->save_button->setEnabled(false);
    ui->firstname_lineEdit->setText(player.firstName());
    ui->lastname_lineEdit->setText(player.lastName());
    ui->email_lineEdit->setText(player.email());
    ui->phone_lineEdit->setText(player.phone());
    ui->username_lineEdit->setText(player.username());
    ui->current_password_lineEdit->setText("");
    ui->new_password_lineEdit->setText("");
    ui->confirm_new_password_lineEdit->setText("");

    QVector<QLineEdit*> edits = {
        ui->firstname_lineEdit, ui->lastname_lineEdit, ui->email_lineEdit, ui->phone_lineEdit,
        ui->username_lineEdit, ui->current_password_lineEdit, ui->new_password_lineEdit, ui->confirm_new_password_lineEdit
    };
    for (QLineEdit* edit : edits) {
        connect(edit, &QLineEdit::textChanged, this, &Change_profile::validateFields);
    }
    connect(ui->save_button, &QPushButton::clicked, this, &Change_profile::onUpdateButtonClicked);

    // فراخوانی اولیه برای بررسی مقادیر پیش‌فرض
    validateFields();
}

Change_profile::~Change_profile()
{
    delete ui;
}

void Change_profile::onUpdateButtonClicked()
{
    QString firstName = ui->firstname_lineEdit->text().trimmed();
    QString lastName = ui->lastname_lineEdit->text().trimmed();
    QString email = ui->email_lineEdit->text().trimmed();
    QString phone = ui->phone_lineEdit->text().trimmed();
    QString newUsername = ui->username_lineEdit->text().trimmed();
    QString oldPassword = ui->current_password_lineEdit->text().trimmed();
    QString newPassword = ui->new_password_lineEdit->text().trimmed();
    QString confirmPassword = ui->confirm_new_password_lineEdit->text().trimmed();

    QStringList errors;
    QString errorMsg;

    if (!(errorMsg = InputValidator::validateName(firstName, "First Name")).isEmpty())
        errors << errorMsg;
    if (!(errorMsg = InputValidator::validateName(lastName, "Last Name")).isEmpty())
        errors << errorMsg;
    if (!(errorMsg = InputValidator::validateEmail(email)).isEmpty())
        errors << errorMsg;
    if (!(errorMsg = InputValidator::validatePhone(phone)).isEmpty())
        errors << errorMsg;
    if (!(errorMsg = InputValidator::validateUsername(newUsername)).isEmpty())
        errors << errorMsg;

    if (oldPassword != player.password()) {
        errors << "Current password is incorrect";
    }

    bool passwordChanged = !newPassword.isEmpty() || !confirmPassword.isEmpty();
    if (passwordChanged) {
        if (!(errorMsg = InputValidator::validatePassword(newPassword)).isEmpty())
            errors << errorMsg;
        if (newPassword != confirmPassword)
            errors << "New password and confirm password do not match";
    }

    if (!errors.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", errors.join("\n"));
        return;
    }

    QString passwordToSend = passwordChanged ? newPassword : player.password();
    QString data = QString("C[CF][%1][%2][%3][%4][%5][%6][%7]\n")
                       .arg(firstName, lastName, phone, email, newUsername, passwordToSend, player.username());
    client->WriteToServer(data);
}

void Change_profile::validateFields()
{
    // بررسی پر بودن فیلدهای غیر رمز عبور
    bool allNonPasswordFilled =
        !ui->firstname_lineEdit->text().trimmed().isEmpty() &&
        !ui->lastname_lineEdit->text().trimmed().isEmpty() &&
        !ui->email_lineEdit->text().trimmed().isEmpty() &&
        !ui->phone_lineEdit->text().trimmed().isEmpty() &&
        !ui->username_lineEdit->text().trimmed().isEmpty() &&
        !ui->current_password_lineEdit->text().trimmed().isEmpty();

    // فعال کردن دکمه ذخیره‌سازی
    ui->save_button->setEnabled(allNonPasswordFilled);

    // لاگ برای دیباگ
    qDebug() << "validateFields: allNonPasswordFilled=" << allNonPasswordFilled
             << ", save_button enabled=" << allNonPasswordFilled;
}
