#ifndef CHOOSE_MODE_H
#define CHOOSE_MODE_H

#include <QMainWindow>

class Client;
class Search_Window;
class game_window;

namespace Ui {
class Choose_mode;
}

class Choose_mode : public QMainWindow
{
    Q_OBJECT

public:
    explicit Choose_mode(Client* client,QWidget *parent = nullptr);
    ~Choose_mode();

    void Player2_Btn_clicked();
    void onPreGameSearching(int waiting, int total);
    void onPreGameFound(const QStringList &opponents);
    void onPreGameFull(const QString &message);

private:
    Ui::Choose_mode *ui;
    Client* client;
    Search_Window* SW;
    game_window* GW;

};

#endif // CHOOSE_MODE_H
