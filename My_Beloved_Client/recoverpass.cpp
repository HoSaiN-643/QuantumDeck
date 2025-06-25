#include "recoverpass.h"
#include "ui_recoverpass.h"

RecoverPass::RecoverPass(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RecoverPass)
{
    ui->setupUi(this);
}

RecoverPass::~RecoverPass()
{
    delete ui;
}
