#include "start.h"
#include "client.h"
#include "connect.h"

Start::Start(QObject *parent)
    : QObject(parent),
    c(new Client(this)),
    window(new Connect(c))
{
    window->show();
}

Start::~Start()
{
    // چون به کنسول یا QMainWindow پاس نکردیم parent،
    // خودمان پاک می‌کنیم:
    delete window;
    // Client به عنوان child این ابجکت ساخته شده، نیازی به delete نیست
}
