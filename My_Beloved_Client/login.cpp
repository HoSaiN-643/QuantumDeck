#include "login.h"       // <<<<<< این خط باید همیشه اول باشه!
#include "ui_login.h"
#include "client.h"
#include "InputValidator.h"
#include <QMessageBox>
#include "mainmenu.h"


Login::Login(Player& player, Client *client, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::Login),
    player(player),
    client(client),
    menuWindow(nullptr)
{
    ui->setupUi(this);
    ui->Login_Btn->setEnabled(false);

    connect(ui->Log_Email_Radio, &QRadioButton::clicked, this, &Login::Update_Login_Btn);
    connect(ui->Log_Uname_Radio, &QRadioButton::clicked, this, &Login::Update_Login_Btn);
    connect(ui->UE_text, &QTextEdit::textChanged, this, &Login::Update_Login_Btn);
    connect(ui->Pwd_text, &QTextEdit::textChanged, this, &Login::Update_Login_Btn);


}

Login::~Login()
{
    delete ui;
}

void Login::Update_Login_Btn()
{
    bool RadioChecked = ui->Log_Email_Radio->isChecked() || ui->Log_Uname_Radio->isChecked();
    bool TextChecked = !ui->Pwd_text->toPlainText().isEmpty() && !ui->UE_text->toPlainText().isEmpty();

    ui->UE_Label_2->setText(ui->Log_Email_Radio->isChecked() ? "Email" : "Username");

    if( RadioChecked && TextChecked) {
        ui->Login_Btn->setEnabled(true);
    }
    else {
        ui->Login_Btn->setEnabled(false);
    }
}

void Login::on_Login_Btn_clicked()
{
    QString inputText = ui->UE_text->toPlainText().trimmed();
    QString password = ui->Pwd_text->toPlainText().trimmed();
    QString type = ui->Log_Email_Radio->isChecked() ? "E" : "U";

    // Re-validate before sending
    QStringList errors;
    if (ui->Log_Email_Radio->isChecked()) {
        if (auto err = InputValidator::validateEmail(inputText); !err.isEmpty())
            errors << "Email: " + err;
    } else if (ui->Log_Uname_Radio->isChecked()) {
        if (inputText.isEmpty())
            errors << "Username cannot be empty";
    }
    if (auto err = InputValidator::validatePassword(password); !err.isEmpty())
        errors << "Password: " + err;

    if (!errors.isEmpty()) {
        QMessageBox::warning(this, "Invalid Login", errors.join("\n"));
        return;
    }

    // Send data in correct format
    QString data = QString("L[%1][%2][%3]").arg(type, inputText, password);
    client->WriteToServer(data);
    // You can also reset the form or add any additional actions here
}

void Login::On_Succesful_Login()
{
    this->close();
    menuWindow = new MainMenu(player,client);
    menuWindow->show();
}
