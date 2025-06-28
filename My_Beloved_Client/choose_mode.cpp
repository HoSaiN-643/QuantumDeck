#include "choose_mode.h"
#include "ui_choose_mode.h"

Choose_mode::Choose_mode(Client* client,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Choose_mode)
    ,client(client)
{
    ui->setupUi(this);
}

Choose_mode::~Choose_mode()
{
    delete ui;
}
