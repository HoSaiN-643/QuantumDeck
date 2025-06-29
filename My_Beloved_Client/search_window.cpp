#include "search_window.h"
#include "ui_search_window.h"
#include <client.h>

Search_Window::Search_Window(Client* client,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Search_Window)
    ,dotCount(0),
    client(client)
{
    ui->setupUi(this);
    ui->Searching_label->setAlignment(Qt::AlignCenter);



    dotTimer = new QTimer(this);
    connect(dotTimer, &QTimer::timeout, this, &Search_Window::updateDots);
    dotTimer->start(500);
    // connect(client,&Client::Waiting,this,[)

}

Search_Window::~Search_Window()
{

    delete ui;
}


void Search_Window::updateDots()
{
    dotCount = (dotCount + 1) % 4;
    QString dots(dotCount, '.');
    ui->Searching_label->setText("Searching for players" + dots);
}
