#ifndef MANAGER_H
#define MANAGER_H
#include <server.h>
#include <memberdatabasemanager.h>

class MANAGER
{
    SERVER* server;
    MemberDatabaseManager* db;
public:
    MANAGER();
};

#endif // MANAGER_H
