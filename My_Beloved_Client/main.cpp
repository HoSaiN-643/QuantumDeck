#include <QApplication>
#include <QFontDatabase>
#include "connect.h"
#include "close.h"
#include "player.h"
#include <QFile>
#include <QMediaPlayer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int fontId = QFontDatabase::addApplicationFont(":/fonts/BebasNeue-Regular.ttf");
    if (fontId != -1) {
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        QFont appFont(fontFamily);
        appFont.setPointSize(14);
        a.setFont(appFont);
        qDebug() << "Font applied successfully:" << fontFamily;
    } else {
        qDebug() << "Failed to load Bebas Neue font, using default font";
        QFont defaultFont;
        a.setFont(defaultFont);
    }
    a.setStyleSheet("QMainWindow { "
                    "background-image: url(:/backgrounds/game_background.jpg); "
                    "background-repeat: no-repeat; "
                    "background-position: center; "
                    "background-color: #f0f0f0; }");
    qDebug() << "Background stylesheet set with game_background.jpg";

    Player player;
    Client client;
    Connect c(player, &client);
    Close closeWindow;
    closeWindow.setClient(&client);
    c.show();
    closeWindow.show();
    return a.exec();
}
