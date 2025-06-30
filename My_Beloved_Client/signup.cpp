#include "signup.h"
#include "ui_signup.h"
#include "client.h"
#include "InputValidator.h"
#include <QMessageBox>
#include <QVector>
#include "login.h"

Signup::Signup(Player& player, Client *client, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::Signup),
    client(client),
    loginWindow(new Login(player, client))
{
    ui->setupUi(this);
    ui->SignUp_Btn->setEnabled(false);

    QVector<QTextEdit*> edits = {
        ui->First_Name_Text, ui->Last_Name_Text, ui->Phone_Text,
        ui->Email_Text, ui->Uname_Text, ui->Pwd_text, ui->Confirm_Pwd_text
    };
    for (QTextEdit* edit : edits)
        connect(edit, &QTextEdit::textChanged, this, &Signup::validateFields);

    connect(ui->SignUp_Btn, &QPushButton::clicked, this, &Signup::SignUp_Btn_clicked);
    client->SetSignup(this); // Inform Client of this instance
}

Signup::~Signup()
{
    delete ui;
}

void Signup::SignUp_Btn_clicked()
{
    QString firstName = ui->First_Name_Text->toPlainText().trimmed();
    QString lastName = ui->Last_Name_Text->toPlainText().trimmed();
    QString email = ui->Email_Text->toPlainText().trimmed();
    QString phone = ui->Phone_Text->toPlainText().trimmed();
    QString username = ui->Uname_Text->toPlainText().trimmed();
    QString password = ui->Pwd_text->toPlainText().trimmed();
    QString confirmPassword = ui->Confirm_Pwd_text->toPlainText().trimmed();

    QString data = QString("S[%1][%2][%3][%4][%5][%6]")
                       .arg(firstName, lastName, email, phone, username, password);
    client->WriteToServer(data);
    qDebug() << "Send sign up request to server :" << data;
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

    ui->SignUp_Btn->setEnabled(allFilled);
}

void Signup::OnSuccesfullSignUp()
{
    this->close();
    loginWindow->show();
}
