#ifndef RECOVERPASS_H
#define RECOVERPASS_H

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
    void on_pushButton_clicked();

private:
    Ui::RecoverPass *ui;
    Client* client;
};

#endif // RECOVERPASS_H