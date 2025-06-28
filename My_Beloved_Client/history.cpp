#include "history.h"
#include "ui_history.h"

History::History(Client* client,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::History)
    ,client(client)
{
    ui->setupUi(this);
}




History::~History()
{
    delete ui;
}
