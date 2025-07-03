#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include "server.h"
#include "memberdatabasemanager.h"

class MANAGER : public QObject
{
    Q_OBJECT

public:
    explicit MANAGER(QObject *parent = nullptr);
    ~MANAGER();

private:
    QString getLocalIPAddress();
    MemberDatabaseManager* db;
    SERVER* server;
};

#endif // MANAGER_H
