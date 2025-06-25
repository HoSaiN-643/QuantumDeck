
#include <log.h>

#include <QApplication>
#include <start.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Start s;

    return a.exec();
}
