#include "log.h"
#include "ui_log.h"
#include "login.h"
#include "signup.h"

Log::Log(Player& player, Client *client, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::Log),
    player(player),
    client(client),
    l(new Login(player, client, this)),
    s(new Signup(player, client, this))
{
    ui->setupUi(this);
    connect(ui->Login_Btn, &QPushButton::clicked, this, &Log::Login_Btn_clicked);
    connect(ui->Signup_btn, &QPushButton::clicked, this, &Log::Signup_btn_clicked);
}

Log::~Log()
{
    delete l;
    delete s;
    delete ui;
}

void Log::Login_Btn_clicked()
{
    this->hide();
    l->show();
}

void Log::Signup_btn_clicked()
{
    this->hide();
    s->show();
}
