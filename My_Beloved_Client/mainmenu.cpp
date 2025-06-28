#include "mainmenu.h"
#include "ui_mainmenu.h"
#include <history.h>
#include <change_profile.h>
#include <choose_mode.h>

MainMenu::MainMenu(Player& player, Client* client, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainMenu),
    player(player),
    client(client),
    historyWindow(new History( client, this)),
    Change_profile_Window(new Change_profile(player, client, this)),
    Choose_mode_Window(new Choose_mode( client, this))
{
    ui->setupUi(this);
    ui->History_Btn->setEnabled(false);
}

Client* client;
History* historyWindow;
Change_profile* Change_profile_Window;
Choose_mode* Choose_mode_Window;

MainMenu::~MainMenu()
{
    delete ui;
}

void MainMenu::on_Choose_Mode_Btn_clicked()
{
    this->close();
    Choose_mode_Window->show();
}


void MainMenu::on_History_Btn_clicked()
{
    this->close();
    historyWindow->show();
}


void MainMenu::on_Change_prof_Btn_clicked()
{
    this->close();
    Change_profile_Window->show();

}

