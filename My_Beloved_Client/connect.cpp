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
    player(player),     // توجه کن! رفرنس رو اینجا ست کن
    client(client),
    logWindow(new Log(player, client, this))
{
    ui->setupUi(this);
    connect(client, &Client::ConnectedToServer, this, &Connect::OnConnected);
    connect(client, &Client::ErrorOccurred, this, &Connect::OnErrorOccurred);
}


Connect::~Connect()
{
    delete ui;
    // logWindow به عنوان child والد (this) پاک می‌شود
}

void Connect::on_Connect_Btn_clicked()
{

    int  port   = ui->Port_line->text().toInt();

    QHostAddress host(QString(ui->Address_line->text().trimmed()));


    qDebug() << "Connecting to" << host.toString() << ":" << port;
    client->ConnectToServer(host, quint16(port));
}

void Connect::OnConnected()
{
    this->hide();
    logWindow->show();
}

void Connect::OnErrorOccurred(const QString &errorString)
{
    QMessageBox::critical(this, "Connection Error", errorString);
}
