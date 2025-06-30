#ifndef CHANGE_PROFILE_H
#define CHANGE_PROFILE_H
#include "client.h"
#include <QMainWindow>
#include <QString>
#include"InputValidator.h"
// Assume Player and Client classes are defined
class Player;
class Client;

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

    // Struct to store original field values
    struct OriginalValues {
        QString phone;
        QString firstname;
        QString lastname;
        QString email;
        QString username;
    } original_values;
};

#endif // CHANGE_PROFILE_H