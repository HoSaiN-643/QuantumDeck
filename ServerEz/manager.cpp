#include "manager.h"
#include <server.h>
#include <memberdatabasemanager.h>
#include <QTcpServer>
#include <QCoreApplication>


MANAGER::MANAGER() {
    QString baseDir = "C:/Users/HoSaiN/Desktop/HoSaiN_Apps_Edits/HoSaiN_Qt/Card game/ServerEz";
    QString dbPath = baseDir + "/members.sqlite";

  QHostAddress address("192.168.88.238");
  db = new MemberDatabaseManager(dbPath);

  server = new SERVER(*db,address.toString());

}
