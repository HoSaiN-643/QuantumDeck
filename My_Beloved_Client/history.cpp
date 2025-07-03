#include "history.h"
#include "ui_history.h"
#include "client.h"
#include "mainmenu.h"
#include <QMessageBox>

History::History(Client* client, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::History),
    client(client)
{
    ui->setupUi(this);
    connect(ui->BackButton, &QPushButton::clicked, this, &History::onBackButtonClicked);
}

History::~History()
{
    delete ui;
}

void History::displayHistory(const QStringList &history)
{
    ui->historyList->clear();
    for (const QString &game : history) {
        ui->historyList->addItem(game);
    }
}

void History::onBackButtonClicked()
{
    this->close();
    client->SetMainMenu(new MainMenu(client->GetPlayer(), client));
    client->GetMainMenu()->show();
}
