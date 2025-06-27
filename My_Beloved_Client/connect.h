#ifndef CONNECT_H
#define CONNECT_H

#include <QMainWindow>
namespace Ui { class connect; }

// فقط اعلام
class Client;
class Log;

class Connect : public QMainWindow
{
    Q_OBJECT

public:
    explicit Connect(Client *client, QWidget *parent = nullptr);
    ~Connect();

private slots:
    void OnConnected();
    void on_Connect_Btn_clicked();
    void OnErrorOccurred(const QString &errorString);

private:
    Ui::connect *ui;
    Client      *client;
    Log         *logWindow;
};

#endif // CONNECT_H
