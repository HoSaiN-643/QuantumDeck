#ifndef CONNECT_H
#define CONNECT_H

#include <QMainWindow>
#include "player.h"  // Include player.h for Player class

namespace Ui { class connect; }

// Forward declarations
class Client;
class Log;

class Connect : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor
    explicit Connect(Client *client, QWidget *parent = nullptr);
    // Destructor
    ~Connect();

private slots:
    // Slot for successful server connection
    void OnConnected();
    // Slot for connect button click
    void on_Connect_Btn_clicked();
    // Slot for handling connection errors
    void OnErrorOccurred(const QString &errorString);

private:
    Ui::connect *ui;
    Client      *client;
    Player      player;  // Added Player member
    Log         *logWindow;
};

#endif // CONNECT_H