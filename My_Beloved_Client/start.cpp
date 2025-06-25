#include "start.h"
#include "connect.h"
#include <client.h>
Start::Start() {
    c = new Client();
    window = new Connect(c);
    window->show();
}
