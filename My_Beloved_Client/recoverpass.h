#ifndef RECOVERPASS_H
#define RECOVERPASS_H

#include <QMainWindow>
#include <QMessageBox>
#include "client.h"
#include "InputValidator.h"

namespace Ui {
class Recoverpass;
}

class Recoverpass : public QMainWindow
{
    Q_OBJECT

public:
    explicit Recoverpass(Client* client, QWidget *parent = nullptr);
    ~Recoverpass();

private slots:
    void onRecoverButtonClicked();

private:
    Ui::Recoverpass *ui;
    Client* client;
};

#endif // RECOVERPASS_H