#include "start.h"
#include "client.h"
#include "connect.h"
#include <QApplication>

Start::Start(QObject *parent)
    : QObject(parent),
    c(new Client(this)),
    window(new Connect(c->GetPlayer(), c))


{
    window->show();


}

Start::~Start()
{
    // پاکسازی منابع
    delete window;

}




