#ifndef RECOVERPASS_H
#define RECOVERPASS_H

#include <QMainWindow>

namespace Ui {
class RecoverPass;
}

class RecoverPass : public QMainWindow
{
    Q_OBJECT

public:
    explicit RecoverPass(QWidget *parent = nullptr);
    ~RecoverPass();

private:
    Ui::RecoverPass *ui;
};

#endif // RECOVERPASS_H
