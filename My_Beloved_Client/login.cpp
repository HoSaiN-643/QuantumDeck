#include "login.h"
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
    connect(ui->Login_Btn, &QPushButton::clicked, this, &Login::Login_Btn_Clicked);
}

Login::~Login()
{
    delete ui;
}

void Login::Open_menu()
{
    if (!menuWindow) {
        menuWindow = new MainMenu(player, client);
    }
    menuWindow->show();
    this->close();
}

void Login::Update_Login_Btn()
{
    bool radioChecked = ui->Log_Email_Radio->isChecked() || ui->Log_Uname_Radio->isChecked();
    bool textFilled = !ui->Pwd_text->toPlainText().trimmed().isEmpty() &&
                      !ui->UE_text->toPlainText().trimmed().isEmpty();

    // Update label based on radio button selection
    if (ui->Log_Email_Radio->isChecked() || ui->Log_Uname_Radio->isChecked()) {
        ui->UE_Label_2->setText(ui->Log_Email_Radio->isChecked() ? "Email" : "Username");
    }

    // Enable login button only if radio button is selected and fields are filled
    ui->Login_Btn->setEnabled(radioChecked && textFilled);
}

void Login::Login_Btn_Clicked()
{
    QString inputText = ui->UE_text->toPlainText().trimmed();
    QString password = ui->Pwd_text->toPlainText().trimmed();
    QString type = ui->Log_Email_Radio->isChecked() ? "E" : "U";

    // Validate inputs and collect errors
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

    // Display errors if any
    if (!errors.isEmpty()) {
        QString errorText = "Please fix the following errors:\n\n";
        errorText += errors.join("\n");
        QMessageBox::warning(this, "Invalid Input", errorText);
        return;
    }

    // Send data to server if inputs are valid
    QString data = QString("L[%1][%2][%3]").arg(type, inputText, password);
    client->WriteToServer(data);
}
