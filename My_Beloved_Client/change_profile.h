#ifndef CHANGE_PROFILE_H
#define CHANGE_PROFILE_H

#include <QMainWindow>
#include "player.h"

class Client;

namespace Ui {
class Change_profile;
}

class Change_profile : public QMainWindow
{
    Q_OBJECT

public:
    explicit Change_profile(Player& player, Client *client, QWidget *parent = nullptr);
    ~Change_profile();

private slots:
    void onUpdateButtonClicked();
    void validateFields();

private:
    Ui::Change_profile *ui;
    Player &player;
    Client *client;
};

#endif // CHANGE_PROFILE_H
