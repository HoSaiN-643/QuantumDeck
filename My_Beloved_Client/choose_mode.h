#ifndef CHOOSE_MODE_H
#define CHOOSE_MODE_H

#include <QMainWindow>

namespace Ui {
class Choose_mode;
}

class Choose_mode : public QMainWindow
{
    Q_OBJECT

public:
    explicit Choose_mode(QWidget *parent = nullptr);
    ~Choose_mode();

private:
    Ui::Choose_mode *ui;
};

#endif // CHOOSE_MODE_H
