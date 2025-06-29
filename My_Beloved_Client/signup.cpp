#include "signup.h"
#include "ui_signup.h"

// Constructor: Initializes the main window and sets up the UI
Signup::Signup(Client* client, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Signup)
    , client(client)
{
    ui->setupUi(this);

    // Connect signup button
    connect(ui->signup_button, &QPushButton::clicked, this, &Signup::onSignupButtonClicked);
}

// Destructor: Cleans up the UI
Signup::~Signup()
{
    delete ui;
}

// Slot for signup button click
void Signup::onSignupButtonClicked()
{
    QString firstname = ui->firstname_lineEdit->text().trimmed();
    QString lastname = ui->lastname_lineEdit->text().trimmed();
    QString email = ui->email_lineEdit->text().trimmed();
    QString phone = ui->phone_lineEdit->text().trimmed();
    QString username = ui->username_lineEdit->text().trimmed();
    QString password = ui->password_lineEdit->text();
    QString confirm_password = ui->confirm_password_lineEdit->text();

    // Validate non-empty fields
    if (firstname.isEmpty()) {
        QMessageBox::warning(this, "Error", "First name cannot be empty.");
        return;
    }
    if (lastname.isEmpty()) {
        QMessageBox::warning(this, "Error", "Last name cannot be empty.");
        return;
    }
    if (email.isEmpty()) {
        QMessageBox::warning(this, "Error", "Email cannot be empty.");
        return;
    }
    if (phone.isEmpty()) {
        QMessageBox::warning(this, "Error", "Phone cannot be empty.");
        return;
    }
    if (username.isEmpty()) {
        QMessageBox::warning(this, "Error", "Username cannot be empty.");
        return;
    }
    if (password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Password cannot be empty.");
        return;
    }

    // Validate name lengths
    if (firstname.length() < 2) {
        QMessageBox::warning(this, "Error", "First name must be at least 2 characters.");
        return;
    }
    if (lastname.length() < 2) {
        QMessageBox::warning(this, "Error", "Last name must be at least 2 characters.");
        return;
    }

    // Validate username length
    if (username.length() < 5) {
        QMessageBox::warning(this, "Error", "Username must be at least 5 characters.");
        return;
    }

    // Validate email and phone
    QString error = InputValidator::validateEmail(email);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "Error", error);
        return;
    }
    error = InputValidator::validatePhone(phone);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "Error", error);
        return;
    }

    // Validate password
    QString error = InputValidator::validatePassword(password);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "Error", "Password: " + error);
        return;
    }
    if (password != confirm_password) {
        QMessageBox::warning(this, "Error", "Password and confirmation do not match.");
        return;
    }

    // Send data to server
    QString message = QString("S[SIGNUP][%1][%2][%3][%4][%5][%6]")
                         .arg(firstname)
                         .arg(lastname)
                         .arg(email)
                         .arg(phone)
                         .arg(username)
                         .arg(password);
    client->WriteToServer(message);

    // Notify success
    QMessageBox::information(this, "Success", "Signup successful.");
}