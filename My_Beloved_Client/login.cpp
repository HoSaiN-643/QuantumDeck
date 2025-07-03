#include "login.h"
#include "ui_login.h"
#include "client.h"
#include "InputValidator.h"
#include <QMessageBox>
#include "mainmenu.h"
#include "recoverpass.h"
#include <QDebug>

Login::Login(Player& player, Client *client, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::Login),
    player(player),
    client(client),
    menuWindow(nullptr),
    RP(nullptr)
{
    ui->setupUi(this);
    ui->Login_Btn->setEnabled(false);

    connect(ui->Log_Email_Radio, &QRadioButton::clicked, this, &Login::Update_Login_Btn);
    connect(ui->Log_Uname_Radio, &QRadioButton::clicked, this, &Login::Update_Login_Btn);
    connect(ui->UE_text, &QTextEdit::textChanged, this, &Login::Update_Login_Btn);
    connect(ui->Pwd_text, &QLineEdit::textChanged, this, &Login::Update_Login_Btn);
    connect(ui->Login_Btn, &QPushButton::clicked, this, &Login::Login_Btn_Clicked);
    connect(ui->Forgot_Btn, &QPushButton::clicked, this, &Login::Recover_Btn_Clicked);

    client->SetLogin(this);
}

Login::~Login()
{
    delete menuWindow;
    delete RP;
    delete ui;
}

void Login::Open_menu()
{
    if (!menuWindow) {
        menuWindow = new MainMenu(player, client);
        client->SetMainMenu(menuWindow);
    }
    menuWindow->show();
    this->hide();
}

void Login::Update_Login_Btn()
{
    bool radioChecked = ui->Log_Email_Radio->isChecked() || ui->Log_Uname_Radio->isChecked();
    bool textFilled = !ui->UE_text->toPlainText().trimmed().isEmpty() &&
                      !ui->Pwd_text->text().trimmed().isEmpty();

    ui->UE_Label_2->setText(ui->Log_Email_Radio->isChecked() ? "Email" : "Username");
    ui->Login_Btn->setEnabled(radioChecked && textFilled);
}

void Login::Login_Btn_Clicked()
{
    QString inputText = ui->UE_text->toPlainText().trimmed();
    QString password = ui->Pwd_text->text().trimmed();
    QString type = ui->Log_Email_Radio->isChecked() ? "E" : "U";

    QStringList errors;
    if (ui->Log_Email_Radio->isChecked()) {
        if (QString err = InputValidator::validateEmail(inputText); !err.isEmpty())
            errors << "Email: " + err;
    } else {
        if (QString err = InputValidator::validateUsername(inputText); !err.isEmpty())
            errors << "Username: " + err;
    }
    if (QString err = InputValidator::validatePassword(password); !err.isEmpty())
        errors << "Password: " + err;

    if (!errors.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", errors.join("\n"));
        return;
    }

    QString data = QString("L[%1][%2][%3]\n").arg(type, inputText, password);
    client->WriteToServer(data);
}

void Login::Recover_Btn_Clicked()
{
    if (!RP) {
        RP = new RecoverPass(client, this);
    }
    RP->show();
    this->hide();
}
