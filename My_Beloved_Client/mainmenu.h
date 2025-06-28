#ifndef MAINMENU_H
#define MAINMENU_H

#include <QMainWindow>
class History;
class Change_profile;
class Choose_mode;

namespace Ui {
class MainMenu;
}

class MainMenu : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainMenu(QWidget *parent = nullptr);
    ~MainMenu();

private:
    Ui::MainMenu *ui;
    History* historyWindow;
    Change_profile* Change_profile_Window;
    Choose_mode* Choose_mode_Window;
};

#endif // MAINMENU_H
