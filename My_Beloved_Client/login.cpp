#include "login.h"
#include "ui_login.h"

// Constructor: Initializes the main window and sets up the UI
Login::Login(Client* client, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Login)
    , client(client)
{
    ui->setupUi(this);

    // Connect login button
    connect(ui->login_button, &QPushButton::clicked, this, &Login::onLoginButtonClicked);
}

// Destructor: Cleans up the UI
Login::~Login()
{
    delete ui;
}

// Slot for login button click
void Login::onLoginButtonClicked()
{
    QString username = ui->username_lineEdit->text().trimmed();
    QString password = ui->password_lineEdit->text();

    // Validate non-empty fields
    if (username.isEmpty()) {
        QMessageBox::warning(this, "Error", "Username cannot be empty.");
        return;
    }
    if (password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Password cannot be empty.");
        return;
    }

    // Validate username length (minimum 5 characters)
    if (username.length() < 5) {
        QMessageBox::warning(this, "Error", "Username must be at least 5 characters.");
        return;
    }

    // Validate password
    QString error = InputValidator::validatePassword(password);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "Error", "Password: " + error);
        return;
    }

    // Send data to server
    QString message = QString("L[LOGIN][%1][%2]").arg(username).arg(password);
    client->WriteToServer(message);

    // Notify success (assuming success)
    QMessageBox::information(this, "Success", "Login successful.");
}