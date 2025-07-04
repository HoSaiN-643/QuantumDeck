#ifndef SEARCH_WINDOW_H
#define SEARCH_WINDOW_H

#include <QMainWindow>
#include <QTimer>

class Client;

namespace Ui {
class Search_Window;
}

class Search_Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Search_Window(Client* client, QWidget *parent = nullptr);
    ~Search_Window();

    void updateSearchStatus(int waiting, int total);

private slots:
    void updateDots();

private:
    Ui::Search_Window *ui;
    QTimer *dotTimer;
    int dotCount;
    int currentPlayers;
    int totalPlayers;
    Client* client;
};

#endif // SEARCH_WINDOW_H
