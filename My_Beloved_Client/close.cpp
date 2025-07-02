#include "close.h"
#include "ui_close.h"

Close::Close(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Close)
{
    ui->setupUi(this);
}

Close::~Close()
{
    delete ui;
}

void Close::on_Close_app_btn_clicked()
{
    QApplication::quit();
}

