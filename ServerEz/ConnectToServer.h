#ifndef CONNECTTOSERVER_H
#define CONNECTTOSERVER_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class ConnectToServerWindow;
}
QT_END_NAMESPACE

class ConnectToServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    ConnectToServerWindow(QWidget *parent = nullptr);
    ~ConnectToServerWindow();

private:
    Ui::ConnectToServerWindow *ui;
};
#endif // CONNECTTOSERVER_H
