#ifndef CHANGE_PROFILE_H
#define CHANGE_PROFILE_H

#include <QMainWindow>

namespace Ui {
class Change_profile;
}

class Change_profile : public QMainWindow
{
    Q_OBJECT

public:
    explicit Change_profile(QWidget *parent = nullptr);
    ~Change_profile();

private:
    Ui::Change_profile *ui;
};

#endif // CHANGE_PROFILE_H
