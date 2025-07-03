#include "recoverpass.h"
#include "ui_recoverpass.h"
#include "client.h"
#include "InputValidator.h"
#include <QMessageBox>
#include <QDebug>

RecoverPass::RecoverPass(Client* client, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::RecoverPass),
    client(client)
{
    ui->setupUi(this);
    connect(ui->RecoverButton, &QPushButton::clicked, this, &RecoverPass::onRecoverButtonClicked);
}

RecoverPass::~RecoverPass()
{
    delete ui;
}

void RecoverPass::onRecoverButtonClicked()
{
    QString phone = ui->PhoneText->toPlainText().trimmed();
    QString error = InputValidator::validatePhone(phone);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "Invalid Phone", error);
        return;
    }
    QString data = QString("R[%1]\n").arg(phone);
    qDebug() << "Sending recover request:" << data;
    client->WriteToServer(data);
    this->close();
}
