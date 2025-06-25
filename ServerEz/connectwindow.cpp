#include "connectwindow.h"
#include "ui_connectwindow.h"
#include <QTcpServer>
#include <server.h>

ConnectWindow::ConnectWindow(SERVER* server ,QWidget *parent )
    : QMainWindow(parent)
    , ui(new Ui::ConnectWindow)
{
    ui->setupUi(this);
    connect(ui->Connect_btn,&QPushButton::clicked,this,&ConnectWindow::connect(server));

}

ConnectWindow::~ConnectWindow()
{
    delete ui;
}

void ConnectWindow::connect(SERVER *server)
{
    server->connect(ui->Address_text->toPlainText(),ui->Port_text->toPlainText().toInt());
}
