#include "manager.h"
#include <server.h>
#include <memberdatabasemanager.h>
#include <QTcpServer>
#include <QCoreApplication>

MANAGER::MANAGER() {
  QString dbPath = QCoreApplication::applicationDirPath() + "/members.sqlite";
  QHostAddress address("192.168.88.238");
  db = new MemberDatabaseManager(dbPath);

  server = new SERVER(*db,address.toString());

}
