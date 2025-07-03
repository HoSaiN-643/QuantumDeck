#include "close.h"
#include "ui_close.h"
#include "connect.h"
#include "signup.h"
#include "login.h"
#include "mainmenu.h"
#include "gamewindow.h"
#include <QApplication>
#include <QDebug>

Close::Close(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Close),
    client(nullptr)
{
    ui->setupUi(this);
    connect(ui->Close_app_btn, &QPushButton::clicked, this, &Close::on_Close_app_btn_clicked);
}

Close::~Close()
{
    delete ui;
}

void Close::setClient(Client *c)
{
    client = c;
}

void Close::on_Close_app_btn_clicked()
{
    if (client) {
        qDebug() << "Closing application initiated...";


        if (client->getSocket() && client->getSocket()->state() == QAbstractSocket::ConnectedState) {
            qDebug() << "Disconnecting from server...";
            client->DisconnectFromServer();
            client->getSocket()->waitForDisconnected(1000);
        }


        qDebug() << "Closing windows...";
        if (auto connectWindow = client->GetConnect()) {
            connectWindow->close();
            connectWindow->deleteLater();
        }
        if (auto signupWindow = client->GetSignup()) {
            signupWindow->close();
            signupWindow->deleteLater();
        }
        if (auto loginWindow = client->GetLogin()) {
            loginWindow->close();
            loginWindow->deleteLater();
        }
        if (auto mainMenuWindow = client->GetMainMenu()) {
            mainMenuWindow->close();
            mainMenuWindow->deleteLater();
        }
        if (auto gameWindow = client->GetGameWindow()) {
            gameWindow->close();
            gameWindow->deleteLater();
        }


        qDebug() << "Forcing application quit...";
        QApplication::closeAllWindows();
        QTimer::singleShot(100, qApp, &QApplication::quit);
    } else {
        qDebug() << "Error: Client pointer is null!";
    }
}
