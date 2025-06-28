#include "mainmenu.h"
#include "ui_mainmenu.h"
#include <history.h>
#include <change_profile.h>
#include <choose_mode.h>

MainMenu::MainMenu(Client* client,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainMenu)
    ,client(client)
    ,historyWindow(new History(client))
    ,Change_profile_Window(new Change_profile(client))
    ,Choose_mode_Window(new Choose_mode(client))

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

}

