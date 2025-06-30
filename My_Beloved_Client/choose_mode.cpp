#include "choose_mode.h"
#include "ui_choose_mode.h"
#include "search_window.h"
#include "client.h"
#include <QMessageBox>

Choose_mode::Choose_mode(Client* client, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::Choose_mode),
    client(client),
    SW(nullptr),
    GW(nullptr)
{
    ui->setupUi(this);
    client->SetChooseMode(this); // Inform Client of this instance
    QObject::connect(ui->Player2_Btn, &QPushButton::clicked, this, &Choose_mode::Player2_Btn_clicked);
}

Choose_mode::~Choose_mode()
{
    delete ui;
}

void Choose_mode::Player2_Btn_clicked()
{
    client->WriteToServer("P[2]");
}

void Choose_mode::onPreGameSearching(int waiting, int total)
{
    if (!SW) {
        SW = new Search_Window(client, this);
    }
    SW->updateWaiting(waiting, total);
    SW->show();
}

void Choose_mode::onPreGameFound(const QStringList &opponents)
{
    // if (SW) {
    //     SW->close();
    //     delete SW;
    //     SW = nullptr;
    // }
    // GW = new game_window(client, opponents, this);
    // GW->show();
    qDebug() << "match found";
}

void Choose_mode::onPreGameFull(const QString &message)
{
    QMessageBox::warning(this, tr("Match Full"), message);
}
