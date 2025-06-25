#include "starter.h"
#include "connect.h"
#include <log.h>
#include <client.h>

Starter::Starter() {
    c = new Client();
    window  = new Connect(c);
    window->show();
}
