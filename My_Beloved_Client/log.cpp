#include "log.h"
#include "ui_log.h"
#include "player.h"
#include "login.h"
#include "signup.h"

// Constructor: Initializes the main window and sets up the UI
Log::Log(Player& player, Client *client, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::Log),
      client(client),
      l(new Login(player, client, this)),
      s(new Signup(player, client, this))
{
    ui->setupUi(this);
    connect(ui->Login_Btn, &QPushButton::clicked, this, &Log::on_Login_Btn_clicked);
    connect(ui->Signup_btn, &QPushButton::clicked, this, &Log::on_Signup_btn_clicked);
}

// Destructor: Cleans up the UI
Log::~Log()
{
    delete ui;
    // l and s are automatically deleted as children
}

// Show the window
void Log::Show()
{
    this->show();
}

// Slot for login button click
void Log::on_Login_Btn_clicked()
{
    this->close();
    l->show();
}

// Slot for signup button click
void Log::on_Signup_btn_clicked()
{
    this->close();
    s->show();
}