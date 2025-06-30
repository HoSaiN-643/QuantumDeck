#include "connect.h"
#include "ui_connect.h"
#include "client.h"
#include "log.h"
#include "player.h"
#include <QHostAddress>
#include <QMessageBox>
#include <QDebug>

// Constructor: Initializes the main window and sets up the UI
Connect::Connect(Client *client, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::connect),
      client(client),
      player(),  // Default-constructed Player
      logWindow(new Log(player, client, this))
{
    ui->setupUi(this);
    connect(ui->Connect_Btn, &QPushButton::clicked, this, &Connect::on_Connect_Btn_clicked);

    connect(client, &Client::ConnectedToServer,
            this, &Connect::OnConnected);
    connect(client, &Client::ErrorOccurred,
            this, &Connect::OnErrorOccurred);
}

// Destructor: Cleans up the UI
Connect::~Connect()
{
    delete ui;
    // logWindow is automatically deleted as a child
}

// Slot for connect button click
void Connect::on_Connect_Btn_clicked()
{
    int port = ui->Port_line->text().toInt();
    QHostAddress host(ui->Address_line->text().trimmed());

    qDebug() << "Connecting to" << host.toString() << ":" << port;
    client->ConnectToServer(host, quint16(port));
}

// Slot for successful server connection
void Connect::OnConnected()
{
    this->hide();
    logWindow->show();
}

// Slot for handling connection errors
void Connect::OnErrorOccurred(const QString &errorString)
{
    QMessageBox::critical(this, "Connection Error", errorString);
}