#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <memberdatabasemanager.h>
#include <server.h>

class MANAGER : public QObject
{
    Q_OBJECT

public:
    explicit MANAGER();

private:
    MemberDatabaseManager* db;
    SERVER* server;

    QString getLocalIPAddress(); // New method to get the system's IP address
};

#endif // MANAGER_H
