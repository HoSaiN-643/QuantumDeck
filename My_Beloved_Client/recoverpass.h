#pragma once
#include <QMainWindow>

class Client;

namespace Ui {
class RecoverPass;
}

class RecoverPass : public QMainWindow
{
    Q_OBJECT

public:
    explicit RecoverPass(Client* client, QWidget *parent = nullptr);
    ~RecoverPass();

private slots:
    void onRecoverButtonClicked();

private:
    Ui::RecoverPass *ui;
    Client* client;
};
