#include <QApplication>
#include <QTableView>
#include <QDebug>
#include <manager.h>
#include <close.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MANAGER BOSS;
    Close c;
    c.show();


    return a.exec();
}
