#include "connect.h"
#include "ui_connect.h"
#include "client.h"
#include "log.h"
#include <QHostAddress>
#include <QMessageBox>
#include <QDebug>

Connect::Connect(Player& player, Client *client, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::connect),
    player(player),
    client(client),
    logWindow(new Log(player, client, this))
{
    ui->setupUi(this);
    client->SetConnect(this); // Inform Client of this instance
    connect(ui->Connect_Btn, &QPushButton::clicked, this, &Connect::Connect_Btn_clicked);
}

Connect::~Connect()
{
    delete ui;
}

void Connect::OnConnected()
{
    this->hide();
    logWindow->show();
}

void Connect::Connect_Btn_clicked()
{
    int port = ui->Port_line->text().toInt();
    QHostAddress host(ui->Address_line->text().trimmed());
    qDebug() << "Connecting to" << host.toString() << ":" << port;
    client->ConnectToServer(host, quint16(port));
}

void Connect::OnErrorOccurred(const QString &errorString)
{
    QMessageBox::critical(this, "Connection Error", errorString);
}
