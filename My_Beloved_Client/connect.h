#ifndef CONNECT_H
#define CONNECT_H

#include <QMainWindow>
#include <client.h>
#include <log.h>

QT_BEGIN_NAMESPACE
namespace Ui { class connect; }
QT_END_NAMESPACE

class Connect : public QMainWindow
{
    Q_OBJECT

public:
    explicit Connect(Client* client, QWidget *parent = nullptr);
    ~Connect();

private slots:
    void OnConnected();
    void on_Connect_Btn_clicked();

private:
    Ui::connect *ui;
    Client* client;
    Log log;
};

#endif // CONNECT_H
