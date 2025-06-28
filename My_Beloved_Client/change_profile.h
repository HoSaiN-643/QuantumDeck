#ifndef CHANGE_PROFILE_H
#define CHANGE_PROFILE_H

#include <QMainWindow>
#include <QMessageBox>
#include "player.h"
#include "client.h"
#include "InputValidator.h"

namespace Ui {
class Change_profile;
}

class Change_profile : public QMainWindow
{
    Q_OBJECT

public:
    explicit Change_profile(Player& player, Client* client, QWidget *parent = nullptr);
    ~Change_profile();

private slots:
    void onFieldChanged();
    void onSaveButtonClicked();

private:
    Ui::Change_profile *ui;
    Player& player;
    Client* client;
};

#endif // CHANGE_PROFILE_H