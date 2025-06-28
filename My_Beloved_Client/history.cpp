#include "history.h"
#include "ui_history.h"

History::History(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::History)
{
    ui->setupUi(this);
}

History::History(Client *client, QWidget *parent)
{

}

History::~History()
{
    delete ui;
}
