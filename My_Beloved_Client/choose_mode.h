#ifndef CHOOSE_MODE_H
#define CHOOSE_MODE_H

#include <QMainWindow>

class Client;
class Search_Window;
class GameWindow;

namespace Ui {
class Choose_mode;
}

class Choose_mode : public QMainWindow
{
    Q_OBJECT

public:
    explicit Choose_mode(Client* client, QWidget *parent = nullptr);
    ~Choose_mode();

public slots:
    void Player2_Btn_clicked();
    void onPreGameSearching(int waiting, int total);
    void onPreGameFound(const QStringList &opponents);

private:
    Ui::Choose_mode *ui;
    Client* client;
    Search_Window* SW;
    GameWindow* GW;
};

#endif // CHOOSE_MODE_H
