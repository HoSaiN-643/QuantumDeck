#include "login.h"
#include "ui_login.h"
#include "client.h"
#include "InputValidator.h"
#include <QMessageBox>

Login::Login(Client *client, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::Login),
      client(client)
{
    ui->setupUi(this);
    ui->Login_Btn->setEnabled(false);

    // اتصال سیگنال‌ها برای اعتبارسنجی و به‌روزرسانی رابط کاربری
    connect(ui->Log_Email_Radio, &QRadioButton::clicked, this, &Login::Update_Login_Btn);
    connect(ui->Log_Uname_Radio, &QRadioButton::clicked, this, &Login::Update_Login_Btn);
    connect(ui->UE_text, &QTextEdit::textChanged, this, &Login::Update_Login_Btn);
    connect(ui->Pwd_text, &QTextEdit::textChanged, this, &Login::Update_Login_Btn);

    // به‌روزرسانی اولیه رابط کاربری
    Update_Login_Btn();
}

Login::~Login()
{
    delete ui;
}

void Login::Update_Login_Btn()
{
    bool radioChecked = ui->Log_Email_Radio->isChecked() || ui->Log_Uname_Radio->isChecked();
    QString inputText = ui->UE_text->toPlainText().trimmed();
    QString password = ui->Pwd_text->toPlainText().trimmed();
    bool textFilled = !inputText.isEmpty() && !password.isEmpty();

    // به‌روزرسانی برچسب بر اساس انتخاب رادیوباتن
    if (ui->Log_Email_Radio->isChecked()) {
        ui->UE_Label_2->setText("Email");
    } else if (ui->Log_Uname_Radio->isChecked()) {
        ui->UE_Label_2->setText("Username");
    } else {
        ui->UE_Label_2->setText("NA");
    }

    // انجام اعتبارسنجی
    bool isValid = true;
    QString errorMessage;

    if (radioChecked && textFilled) {
        if (ui->Log_Email_Radio->isChecked()) {
            QString emailError = InputValidator::validateEmail(inputText);
            if (!emailError.isEmpty()) {
                isValid = false;
                errorMessage = emailError;
                ui->UE_text->setStyleSheet("border: 1px solid red;");
            } else {
                ui->UE_text->setStyleSheet("");
            }
        } else if (ui->Log_Uname_Radio->isChecked()) {
            if (inputText.isEmpty()) {
                isValid = false;
                errorMessage = "نام کاربری نمی‌تواند خالی باشد";
                ui->UE_text->setStyleSheet("border: 1px solid red;");
            } else {
                ui->UE_text->setStyleSheet("");
            }
        }

        QString passwordError = InputValidator::validatePassword(password);
        if (!passwordError.isEmpty()) {
            isValid = false;
            if (!errorMessage.isEmpty()) {
                errorMessage += "; " + passwordError;
            } else {
                errorMessage = passwordError;
            }
            ui->Pwd_text->setStyleSheet("border: 1px solid red;");
        } else {
            ui->Pwd_text->setStyleSheet("");
        }
    } else {
        isValid = false;
        if (!radioChecked) {
            errorMessage = "لطفاً روش ورود را انتخاب کنید";
        } else if (!textFilled) {
            errorMessage = "لطفاً همه فیلدها را پر کنید";
        }
    }

    // فعال‌سازی دکمه ورود فقط در صورت معتبر بودن همه ورودی‌ها
    ui->Login_Btn->setEnabled(isValid);

    // نمایش پیام‌های خطا در نوار وضعیت
    if (!errorMessage.isEmpty()) {
        ui->statusbar->showMessage(errorMessage);
    } else {
        ui->statusbar->clearMessage();
    }
}

void Login::on_Login_Btn_clicked()
{
    QString inputText = ui->UE_text->toPlainText().trimmed();
    QString password = ui->Pwd_text->toPlainText().trimmed();
    QString type = ui->Log_Email_Radio->isChecked() ? "E" : "U";

    // اعتبارسنجی ورودی‌ها
    if (ui->Log_Email_Radio->isChecked()) {
        QString emailError = InputValidator::validateEmail(inputText);
        if (!emailError.isEmpty()) {
            QMessageBox::warning(this, "خطای ورود", "ایمیل نامعتبر: " + emailError);
            return;
        }
    } else if (ui->Log_Uname_Radio->isChecked()) {
        if (inputText.isEmpty()) {
            QMessageBox::warning(this, "خطای ورود", "نام کاربری نمی‌تواند خالی باشد");
            return;
        }
    }

    QString passwordError = InputValidator::validatePassword(password);
    if (!passwordError.isEmpty()) {
        QMessageBox::warning(this, "خطای ورود", "رمز عبور نامعتبر: " + passwordError);
        return;
    }

    // قالب‌بندی داده‌ها برای ارسال به سرور
    QString data = QString("L %1 %2 %3").arg(type).arg(inputText).arg(password);

    // ارسال داده‌ها به سرور
    client->WriteToServer(data);
}