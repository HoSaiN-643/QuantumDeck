#include "choose_mode.h"
#include "ui_choose_mode.h"
#include "client.h"
#include "search_window.h"
#include "gamewindow.h"
#include <QMessageBox>
#include <QDebug>

Choose_mode::Choose_mode(Client* client, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::Choose_mode),
    client(client),
    SW(nullptr),
    GW(nullptr)
{
    ui->setupUi(this);
    client->SetChooseMode(this);
    connect(ui->Player2_Btn, &QPushButton::clicked, this, &Choose_mode::Player2_Btn_clicked);
}

Choose_mode::~Choose_mode()
{
    delete SW;
    delete GW;
    delete ui;
}

void Choose_mode::Player2_Btn_clicked()
{
    QString data = "P[2]\n";
    client->WriteToServer(data);
    this->hide();
    if (!SW) {
        SW = new Search_Window(client, this);
    }
    SW->show();
}

void Choose_mode::onPreGameSearching(int waiting, int total)
{
    if (SW) {
        SW->updateSearchStatus(waiting, total);
    }
}

void Choose_mode::onPreGameFound(const QStringList &opponents)
{
    if (SW) {
        SW->hide();
    }
    if (!GW) {
        GW = new GameWindow(client, this);
        client->SetGameWindow(GW);
    }
    GW->show();
    this->hide();
}
