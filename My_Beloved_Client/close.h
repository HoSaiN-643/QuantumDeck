#ifndef CLOSE_H
#define CLOSE_H

#include <QMainWindow>
#include "client.h"

namespace Ui {
class Close;
}

class Close : public QMainWindow
{
    Q_OBJECT

public:
    explicit Close(QWidget *parent = nullptr);
    ~Close();
    void setClient(Client *c); // متد برای تنظیم اشاره‌گر Client

private slots:
    void on_Close_app_btn_clicked();

private:
    Ui::Close *ui;
    Client *client; // اشاره‌گر به Client
};

#endif // CLOSE_H
