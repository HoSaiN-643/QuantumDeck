#include "search_window.h"
#include "ui_search_window.h"
#include "client.h"
#include <QDebug>

Search_Window::Search_Window(Client* client, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::Search_Window),
    dotCount(0),
    client(client),
    currentPlayers(0),
    totalPlayers(0)
{
    ui->setupUi(this);
    ui->Searching_label->setAlignment(Qt::AlignCenter);
    dotTimer = new QTimer(this);
    connect(dotTimer, &QTimer::timeout, this, &Search_Window::updateDots);
    dotTimer->start(500);
    ui->Searching_label->setText("Waiting for players...");
    qDebug() << "Search_Window initialized";
}

Search_Window::~Search_Window()
{
    dotTimer->stop();
    delete dotTimer;
    delete ui;
    qDebug() << "Search_Window destroyed";
}

void Search_Window::updateSearchStatus(int waiting, int total)
{
    currentPlayers = waiting;
    totalPlayers = total;
    updateDots();
    qDebug() << "Search_Window updated, waiting:" << waiting << ", total:" << total;
}

void Search_Window::updateDots()
{
    dotCount = (dotCount + 1) % 4;
    QString dots(dotCount, '.');
    ui->Searching_label->setText(QString("Waiting for players: %1/%2%3").arg(currentPlayers).arg(totalPlayers).arg(dots));
}
