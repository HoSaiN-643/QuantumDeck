#include "change_profile.h"
#include "ui_change_profile.h"

Change_profile::Change_profile(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Change_profile)
{
    ui->setupUi(this);
}

Change_profile::~Change_profile()
{
    delete ui;
}
