#include <QApplication>
#include "start.h"
#include <close.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Start s;
    Close c;
    c.show();
    return a.exec();
}
