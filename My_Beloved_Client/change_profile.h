#pragma once
#include <QMainWindow>
#include "player.h"
#include "client.h"

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

    // Structure to store original values for comparison
    struct OriginalValues {
        QString phone;
        QString firstname;
        QString lastname;
        QString email;
        QString username;
    } original_values;
};
