#include "mainmenu.h"
#include "ui_mainmenu.h"
#include "history.h"
#include "change_profile.h"
#include "choose_mode.h"
#include <QDebug>
#include "client.h"

MainMenu::MainMenu(Player& player, Client* client, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainMenu),
    player(player),
    client(client),
    historyWindow(new History(client, this)),
    Change_profile_Window(new Change_profile(player, client, this)),
    Choose_mode_Window(new Choose_mode(client, this))
{
    ui->setupUi(this);
    ui->History_Btn->setEnabled(true);
    connect(ui->Choose_Mode_Btn, &QPushButton::clicked, this, &MainMenu::Choose_Mode_Btn_clicked);
    connect(ui->History_Btn, &QPushButton::clicked, this, &MainMenu::History_Btn_clicked);
    connect(ui->Change_prof_Btn, &QPushButton::clicked, this, &MainMenu::Change_prof_Btn_clicked);
    client->SetChangeProfile(Change_profile_Window);
    client->SetMainMenu(this);
}

MainMenu::~MainMenu()
{
    delete historyWindow;
    delete Change_profile_Window;
    delete Choose_mode_Window;
    delete ui;
}

void MainMenu::displayGameHistory(const QStringList &history)
{
    QString historyText = "Game History:\n" + history.join("\n");
    qDebug() << historyText;
}

void MainMenu::Choose_Mode_Btn_clicked()
{
    this->hide();
    Choose_mode_Window->show();
}

void MainMenu::History_Btn_clicked()
{
    this->hide();
    historyWindow->show();
    client->SetHistory(historyWindow);
}

void MainMenu::Change_prof_Btn_clicked()
{
    this->hide();
    Change_profile_Window->show();
}
