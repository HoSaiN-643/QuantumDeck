#include "choose_mode.h"
#include "ui_choose_mode.h"
#include "search_window.h"
#include "client.h"
#include <QMessageBox>


Choose_mode::Choose_mode(Client* client,QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::Choose_mode),
    client(client),
    SW(nullptr),
    GW(nullptr)
{
    ui->setupUi(this);

    // کانکت سیگنال‌های پری‌گیم به اسلات‌های خودمان
    connect(client, &Client::PreGameSearching,
            this,   &Choose_mode::onPreGameSearching);
    connect(client, &Client::PreGameFound,
            this,   &Choose_mode::onPreGameFound);
    connect(client, &Client::PreGameFull,
            this,   &Choose_mode::onPreGameFull);
    QObject::connect(ui->Player2_Btn, &QPushButton::clicked, this, &Choose_mode::Player2_Btn_clicked);
}

Choose_mode::~Choose_mode()
{
    delete ui;
}



void Choose_mode::Player2_Btn_clicked()
{
    // درخواست پیش‌گیم را به سرور می‌فرستیم
    client->WriteToServer("P[2]");
    // منتظر می‌مانیم تا خودش سیگنال Searching یا FULL ارسال کند
}

void Choose_mode::onPreGameSearching(int waiting, int total)
{
    // اگر پنجرهٔ Search_Window نداریم بسازیم
    if (!SW) {
        SW = new Search_Window(client, this);
    }
    SW->updateWaiting(waiting, total); // فرض کنید این متد label را آپدیت کند
    SW->show();
}

void Choose_mode::onPreGameFound(const QStringList &opponents)
{
    if (SW) {
        SW->close();
        delete SW; SW = nullptr;
    }
    // پنجرۀ اصلیِ بازی را باز می‌کنیم
    // GW = new game_window(client, opponents, this);
    // GW->show();
}

void Choose_mode::onPreGameFull(const QString &message)
{
    QMessageBox::warning(this,
                         tr("Match Full"),
                         message);
}

