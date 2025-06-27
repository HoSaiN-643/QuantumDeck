#include "signup.h"
#include "ui_signup.h"
#include "client.h"
#include <QMessageBox>

Signup::Signup(Client *client, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::Signup),
    client(client)
{
    ui->setupUi(this);
}

Signup::~Signup()
{
    delete ui;
}

void Signup::on_Register_Btn_clicked()
{
    // QString u = ui->Username_line->text();
    // QString p = ui->Password_line->text();
    // if (u.isEmpty() || p.isEmpty()) {
    //     QMessageBox::warning(this, "Warning", "Empty fields");
    //     return;
    // }
    // client->sendSignup(u,p);
}

