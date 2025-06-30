#include "login.h"
#include "ui_login.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>

// Constructor: Initializes the main window and sets up the UI
Login::Login(Client* client, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Login)
    , client(client)
{
    ui->setupUi(this);

    // Connect login button
    connect(ui->login_button, &QPushButton::clicked, this, &Login::onLoginButtonClicked);
    // Connect forgot password button
    connect(ui->forgotPassword_button, &QPushButton::clicked, this, &Login::onForgotPasswordButtonClicked);

    // Add validator for phone number
    QRegularExpression phone_rx("09\\d{9}");
    ui->phone_lineEdit->setValidator(new QRegularExpressionValidator(phone_rx, this));
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

// Slot for forgot password button click
void Login::onForgotPasswordButtonClicked()
{
    QString phone = ui->phone_lineEdit->text().trimmed();
    QString new_password = ui->newPassword_lineEdit->text();
    QString confirm_new_password = ui->confirmNewPassword_lineEdit->text();

    // Validate non-empty fields
    if (phone.isEmpty()) {
        QMessageBox::warning(this, "Error", "Phone number cannot be empty.");
        return;
    }
    if (new_password.isEmpty()) {
        QMessageBox::warning(this, "Error", "New password cannot be empty.");
        return;
    }
    if (confirm_new_password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Confirm new password cannot be empty.");
        return;
    }

    // Validate phone number format
    QRegularExpression phone_rx("09\\d{9}");
    if (!phone_rx.match(phone).hasMatch()) {
        QMessageBox::warning(this, "Error", "Invalid phone number format. Use 09 followed by 9 digits.");
        return;
    }

    // Validate new password
    QString error = InputValidator::validatePassword(new_password);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "Error", "New password: " + error);
        return;
    }

    // Check if new password and confirmation match
    if (new_password != confirm_new_password) {
        QMessageBox::warning(this, "Error", "New password and confirmation do not match.");
        return;
    }

    // Send forgot password request to server
    QString message = QString("F[FORGOT][%1][%2]").arg(phone).arg(new_password);
    client->WriteToServer(message);

    // Notify success (assuming server handles validation)
    QMessageBox::information(this, "Success", "Password reset request sent successfully.");
}