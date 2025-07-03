#pragma once
#include <QMainWindow>
#include "player.h"
#include "client.h"

class History;
class Change_profile;
class Choose_mode;

namespace Ui {
class MainMenu;
}

class MainMenu : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainMenu(Player& player, Client* client, QWidget *parent = nullptr);
    ~MainMenu();
    void displayGameHistory(const QStringList &history);

private slots:
    void Choose_Mode_Btn_clicked();
    void History_Btn_clicked();
    void Change_prof_Btn_clicked();

private:
    Ui::MainMenu *ui;
    Client* client;
    Player& player;
    History* historyWindow;
    Change_profile* Change_profile_Window;
    Choose_mode* Choose_mode_Window;
};
