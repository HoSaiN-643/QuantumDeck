#include "signup.h"
#include "ui_signup.h"
#include "client.h"
#include "InputValidator.h"
#include <QMessageBox>
#include <QVector>
#include "log.h"
#include <QDebug>

Signup::Signup(Player& player, Client *client, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::Signup),
    client(client),
    player(player)
{
    ui->setupUi(this);
    ui->SignUp_Btn->setEnabled(false);

    QVector<QTextEdit*> edits = {
        ui->First_Name_Text, ui->Last_Name_Text, ui->Phone_Text,
        ui->Email_Text, ui->Uname_Text, ui->Pwd_text, ui->Confirm_Pwd_text
    };
    for (QTextEdit* edit : edits)
        connect(edit, &QTextEdit::textChanged, this, &Signup::validateFields);

    connect(ui->SignUp_Btn, &QPushButton::clicked, this, &Signup::SignUp_Btn_Clicked);
    client->SetSignup(this);
}

Signup::~Signup()
{
    delete ui;
}

void Signup::SignUp_Btn_Clicked()
{
    QString firstName = ui->First_Name_Text->toPlainText().trimmed();
    QString lastName = ui->Last_Name_Text->toPlainText().trimmed();
    QString phone = ui->Phone_Text->toPlainText().trimmed();
    QString email = ui->Email_Text->toPlainText().trimmed();
    QString username = ui->Uname_Text->toPlainText().trimmed();
    QString password = ui->Pwd_text->toPlainText().trimmed();
    QString confirmPassword = ui->Confirm_Pwd_text->toPlainText().trimmed();

    QStringList errors;
    if (QString err = InputValidator::validateName(firstName, "First Name"); !err.isEmpty())
        errors << err;
    if (QString err = InputValidator::validateName(lastName, "Last Name"); !err.isEmpty())
        errors << err;
    if (QString err = InputValidator::validatePhone(phone); !err.isEmpty())
        errors << err;
    if (QString err = InputValidator::validateEmail(email); !err.isEmpty())
        errors << err;
    if (QString err = InputValidator::validateUsername(username); !err.isEmpty())
        errors << err;
    if (QString err = InputValidator::validatePassword(password); !err.isEmpty())
        errors << err;
    if (password != confirmPassword)
        errors << "Passwords do not match";

    if (!errors.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", errors.join("\n"));
        return;
    }

    QString data = QString("S[%1][%2][%3][%4][%5][%6]\n")
                       .arg(firstName, lastName, phone, email, username, password);
    client->WriteToServer(data);
}

void Signup::validateFields()
{
    bool allFilled =
        !ui->First_Name_Text->toPlainText().trimmed().isEmpty() &&
        !ui->Last_Name_Text->toPlainText().trimmed().isEmpty() &&
        !ui->Phone_Text->toPlainText().trimmed().isEmpty() &&
        !ui->Email_Text->toPlainText().trimmed().isEmpty() &&
        !ui->Uname_Text->toPlainText().trimmed().isEmpty() &&
        !ui->Pwd_text->toPlainText().trimmed().isEmpty() &&
        !ui->Confirm_Pwd_text->toPlainText().trimmed().isEmpty();

    bool allValid = true;
    if (allFilled) {
        if (!InputValidator::validateName(ui->First_Name_Text->toPlainText().trimmed(), "First Name").isEmpty())
            allValid = false;
        if (!InputValidator::validateName(ui->Last_Name_Text->toPlainText().trimmed(), "Last Name").isEmpty())
            allValid = false;
        if (!InputValidator::validatePhone(ui->Phone_Text->toPlainText().trimmed()).isEmpty())
            allValid = false;
        if (!InputValidator::validateEmail(ui->Email_Text->toPlainText().trimmed()).isEmpty())
            allValid = false;
        if (!InputValidator::validateUsername(ui->Uname_Text->toPlainText().trimmed()).isEmpty())
            allValid = false;
        if (!InputValidator::validatePassword(ui->Pwd_text->toPlainText().trimmed()).isEmpty())
            allValid = false;
        if (ui->Pwd_text->toPlainText().trimmed() != ui->Confirm_Pwd_text->toPlainText().trimmed())
            allValid = false;
    } else {
        allValid = false;
    }

    ui->SignUp_Btn->setEnabled(allFilled && allValid);
}

void Signup::OnSuccesfullSignUp()
{
    this->hide();
    Log* logWindow = new Log(player, client);
    logWindow->show();
}
