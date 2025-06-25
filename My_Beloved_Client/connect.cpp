#include "connect.h"
#include "ui_connect.h"
#include <QMessageBox>
#include <QHostAddress>
#include <QDebug>

Connect::Connect(Client* client, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::connect),
    client(client)
{
    ui->setupUi(this);
    // ui->Address_line->setInputMask("000.000.000.000");

    connect(client, &Client::ConnectedToServer,
            this,   &Connect::OnConnected);
}

Connect::~Connect()
{
    delete ui;
}

void Connect::OnConnected()
{
    log.show();
    this->close();
}

void Connect::on_Connect_Btn_clicked()
{
    QString addrText = ui->Address_line->text().trimmed();
    bool okPort = false;
    int port = ui->Port_line->text().toInt(&okPort);

    if (addrText.isEmpty()) {
        QMessageBox::critical(this, "Error", "Address is empty");
        return;
    }

    QHostAddress host;
    if (!host.setAddress(addrText)) {
        QMessageBox::critical(this, "Error", "Invalid IP address");
        return;
    }

    if (!okPort || port < 1 || port > 65535) {
        QMessageBox::critical(this, "Error", "Invalid port number");
        return;
    }

    qDebug() << "Connecting to" << host.toString() << ":" << port;
    client->ConnectToServer(host, port);
}
