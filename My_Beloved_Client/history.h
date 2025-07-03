#ifndef HISTORY_H
#define HISTORY_H

#include <QMainWindow>

class Client;

namespace Ui {
class History;
}

class History : public QMainWindow
{
    Q_OBJECT

public:
    explicit History(Client* client, QWidget *parent = nullptr);
    ~History();

    void displayHistory(const QStringList &history);

private slots:
    void onBackButtonClicked();

private:
    Ui::History *ui;
    Client *client;
};

#endif // HISTORY_H
