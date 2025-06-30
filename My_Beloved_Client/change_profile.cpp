#include "player.h"
#include "change_profile.h"
#include "ui_change_profile.h"
#include "client.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QTimer>

// Constructor: Initializes the main window and sets up the UI
Change_profile::Change_profile(Player& player, Client* client, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Change_profile)
    , player(player)
    , client(client)
{
    if (!client) {
        QMessageBox::critical(this, "Error", "No connection to the server.");
        close();
        return;
    }

    ui->setupUi(this);

    // Store original values for comparison
    original_values.phone = player.phone();
    original_values.firstname = player.firstName();
    original_values.lastname = player.lastName();
    original_values.email = player.email();
    original_values.username = player.username();

    // Populate fields with player data
    ui->phone_lineEdit->setText(player.phone());
    ui->firstname_lineEdit->setText(player.firstName());
    ui->lastname_lineEdit->setText(player.lastName());
    ui->email_lineEdit->setText(player.email());
    ui->username_lineEdit->setText(player.username());

    // Clear password fields
    ui->current_password_lineEdit->clear();
    ui->new_password_lineEdit->clear();
    ui->confirm_new_password_lineEdit->clear();

    // Disable save button initially
    ui->save_button->setEnabled(false);

    // Add local validators
    QRegularExpression email_rx("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    ui->email_lineEdit->setValidator(new QRegularExpressionValidator(email_rx, this));
    ui->phone_lineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("09\\d{9}"), this));

    // Connect textChanged signals
    connect(ui->phone_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->firstname_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->lastname_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->email_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->username_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->current_password_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->new_password_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);
    connect(ui->confirm_new_password_lineEdit, &QLineEdit::textChanged, this, &Change_profile::onFieldChanged);

    // Connect save button
    connect(ui->save_button, &QPushButton::clicked, this, &Change_profile::onSaveButtonClicked);
}

// Destructor: Cleans up the UI
Change_profile::~Change_profile()
{
    delete ui;
}

// Slot for field changes
void Change_profile::onFieldChanged()
{
    // Check if any field has changed
    bool has_changes =
        ui->phone_lineEdit->text().trimmed() != original_values.phone ||
        ui->firstname_lineEdit->text().trimmed() != original_values.firstname ||
        ui->lastname_lineEdit->text().trimmed() != original_values.lastname ||
        ui->email_lineEdit->text().trimmed() != original_values.email ||
        ui->username_lineEdit->text().trimmed() != original_values.username ||
        !ui->current_password_lineEdit->text().isEmpty() ||
        !ui->new_password_lineEdit->text().isEmpty() ||
        !ui->confirm_new_password_lineEdit->text().isEmpty();

    ui->save_button->setEnabled(has_changes);
}

// Slot for save button click
void Change_profile::onSaveButtonClicked()
{
    // Disable save button during processing
    ui->save_button->setEnabled(false);

    // Retrieve field values
    QString phone = ui->phone_lineEdit->text().trimmed();
    QString firstname = ui->firstname_lineEdit->text().trimmed();
    QString lastname = ui->lastname_lineEdit->text().trimmed();
    QString email = ui->email_lineEdit->text().trimmed();
    QString username = ui->username_lineEdit->text().trimmed();
    QString current_password = ui->current_password_lineEdit->text();
    QString new_password = ui->new_password_lineEdit->text();
    QString confirm_new_password = ui->confirm_new_password_lineEdit->text();

    // Validate non-empty fields
    if (phone.isEmpty()) {
        QMessageBox::warning(this, "Error", "Phone cannot be empty.");
        ui->save_button->setEnabled(true);
        return;
    }
    if (firstname.isEmpty()) {
        QMessageBox::warning(this, "Error", "First name cannot be empty.");
        ui->save_button->setEnabled(true);
        return;
    }
    if (lastname.isEmpty()) {
        QMessageBox::warning(this, "Error", "Last name cannot be empty.");
        ui->save_button->setEnabled(true);
        return;
    }
    if (email.isEmpty()) {
        QMessageBox::warning(this, "Error", "Email cannot be empty.");
        ui->save_button->setEnabled(true);
        return;
    }
    if (username.isEmpty()) {
        QMessageBox::warning(this, "Error", "Username cannot be empty.");
        ui->save_button->setEnabled(true);
        return;
    }

    // Validate name lengths
    if (firstname.length() < 2) {
        QMessageBox::warning(this, "Error", "First name must be at least 2 characters.");
        ui->save_button->setEnabled(true);
        return;
    }
    if (lastname.length() < 2) {
        QMessageBox::warning(this, "Error", "Last name must be at least 2 characters.");
        ui->save_button->setEnabled(true);
        return;
    }

    // Validate username length
    if (username.length() < 5) {
        QMessageBox::warning(this, "Error", "Username must be at least 5 characters.");
        ui->save_button->setEnabled(true);
        return;
    }

    // Validate email and phone with InputValidator
    QString error = InputValidator::validateEmail(email);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "Error", error);
        ui->save_button->setEnabled(true);
        return;
    }
    error = InputValidator::validatePhone(phone);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "Error", error);
        ui->save_button->setEnabled(true);
        return;
    }

    // Handle password changes
    QString password_to_send = player.password();
    if (!current_password.isEmpty() || !new_password.isEmpty() || !confirm_new_password.isEmpty()) {
        // All password fields must be filled
        if (current_password.isEmpty() || new_password.isEmpty() || !confirm_new_password.isEmpty()) {
            QMessageBox::warning(this, "Error", "All password fields must be filled to change the password.");
            ui->save_button->setEnabled(true);
            return;
        }

        // Verify current password (plaintext comparison is insecure)
        if (current_password != player.password()) {
            QMessageBox::warning(this, "Error", "Current password is incorrect.");
            ui->save_button->setEnabled(true);
            return;
        }

        // Check new password match
        if (new_password != confirm_new_password) {
            QMessageBox::warning(this, "Error", "New password and confirmation do not match.");
            ui->save_button->setEnabled(true);
            return;
        }

        // Validate new password
        error = InputValidator::validatePassword(new_password);
        if (!error.isEmpty()) {
            QMessageBox::warning(this, "Error", "New password: " + error);
            ui->save_button->setEnabled(true);
            return;
        }

        password_to_send = new_password;
    }

    // Send data to server
    QString message = QString("C[CF][%1][%2][%3][%4][%5][%6]")
                         .arg(firstname, lastname, email, phone, username, password_to_send);
    client->WriteToServer(message);  // Removed bool assignment, assuming void return
    // Since WriteToServer returns void, assume success and handle errors via signals
    // If you need success feedback, modify Client to return bool

    // Update player data
    player.setPhone(phone);
    player.setFirstName(firstname);
    player.setLastName(lastname);
    player.setEmail(email);
    player.setUsername(username);
    if (password_to_send != player.password()) {
        player.setPassword(password_to_send);
    }

    // Update original values
    original_values.phone = phone;
    original_values.firstname = firstname;
    original_values.lastname = lastname;
    original_values.email = email;
    original_values.username = username;

    // Show success message
    QMessageBox::information(this, "Success", "Changes saved successfully.");
    ui->save_button->setEnabled(false);
}