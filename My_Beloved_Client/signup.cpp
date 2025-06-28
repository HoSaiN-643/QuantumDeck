#include "signup.h"
#include "ui_signup.h"
#include "client.h"
#include <QMessageBox>
#include <QRegularExpression>

Signup::Signup(Client *client, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::Signup),
      client(client)
{
    ui->setupUi(this);
    ui->SignUp_Btn->setEnabled(false);

    connect(ui->First_Name_Text, &QTextEdit::textChanged, this, &Signup::checkFields);
    connect(ui->Last_Name_Text, &QTextEdit::textChanged, this, &Signup::checkFields);
    connect(ui->Phone_Text, &QTextEdit::textChanged, this, &Signup::checkFields);
    connect(ui->Email_Text, &QTextEdit::textChanged, this, &Signup::checkFields);
    connect(ui->Uname_Text, &QTextEdit::textChanged, this, &Signup::checkFields);
    connect(ui->Pwd_text, &QTextEdit::textChanged, this, &Signup::checkFields);
    connect(ui->Confirm_Pwd_text, &QTextEdit::textChanged, this, &Signup::checkFields);
    connect(ui->SignUp_Btn, &QPushButton::clicked, this, &Signup::on_SignUp_Btn_clicked);
}

Signup::~Signup()
{
    delete ui;
}

void Signup::checkFields()
{
    bool allFilled = true;
    if (ui->First_Name_Text->toPlainText().isEmpty()) allFilled = false;
    if (ui->Last_Name_Text->toPlainText().isEmpty()) allFilled = false;
    if (ui->Phone_Text->toPlainText().isEmpty()) allFilled = false;
    if (ui->Email_Text->toPlainText().isEmpty()) allFilled = false;
    if (ui->Uname_Text->toPlainText().isEmpty()) allFilled = false;
    if (ui->Pwd_text->toPlainText().isEmpty()) allFilled = false;
    if (ui->Confirm_Pwd_text->toPlainText().isEmpty()) allFilled = false;
    ui->SignUp_Btn->setEnabled(allFilled);
}

void Signup::on_SignUp_Btn_clicked()
{
    QString first = ui->First_Name_Text->toPlainText().trimmed();
    QString last = ui->Last_Name_Text->toPlainText().trimmed();
    QString phone = ui->Phone_Text->toPlainText().trimmed();
    QString email = ui->Email_Text->toPlainText().trimmed();
    QString username = ui->Uname_Text->toPlainText().trimmed();
    QString pwd = ui->Pwd_text->toPlainText().trimmed();
    QString confirm_pwd = ui->Confirm_Pwd_text->toPlainText().trimmed();

    if (first.isEmpty() || last.isEmpty() || phone.isEmpty() || email.isEmpty() ||
        username.isEmpty() || pwd.isEmpty() || confirm_pwd.isEmpty()) {
        QMessageBox::warning(this, "خطا", "تمامی فیلدها باید پر شوند.");
        return;
    }

    if (pwd != confirm_pwd) {
        QMessageBox::warning(this, "خطا", "رمزهای عبور مطابقت ندارند.");
        return;
    }

    QRegularExpression emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    if (!emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, "خطا", "فرمت ایمیل نامعتبر است.");
        return;
    }

    QRegularExpression phoneRegex("^\\d{3}\\.\\d{3}\\.\\d{3}\\.\\d{3}$");
    if (!phoneRegex.match(phone).hasMatch()) {
        QMessageBox::warning(this, "خطا", "فرمت شماره تلفن باید به صورت 000.000.000.000 باشد.");
        return;
    }

    QString data = "S[" + first + "][" + last + "][" + phone + "][" + email + "][" + username + "][" + pwd + "]";
    client->WriteToServer(data);
    QMessageBox::information(this, "موفقیت", "درخواست ثبت‌نام ارسال شد.");
}