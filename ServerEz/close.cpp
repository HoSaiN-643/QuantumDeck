#include "close.h"
#include "ui_close.h"
#include <QTimer>

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

void Close::on_pushButton_clicked()
{
    QApplication::closeAllWindows();
    QTimer::singleShot(100, qApp, &QApplication::quit);
}

