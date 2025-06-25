#include "ConnectToServer.h"
#include "ui_ConnectToServer.h"
#include <QTcpServer>

ConnectToServerWindow::ConnectToServerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

   //اضافه کردن یک کد برای گرفتن ادرس فعلی اینترنت و نمایش اولیه در یو ای
    ui->Port_text->setPlaceholderText("8888");

    connect(ui->Connect_btn,&QPushButton::clicked,this,);
}

ConnectToServerWindow::~ConnectToServerWindow()
{
    delete ui;
}
