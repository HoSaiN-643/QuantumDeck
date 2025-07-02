#ifndef CLOSE_H
#define CLOSE_H

#include <QMainWindow>

namespace Ui {
class Close;
}

class Close : public QMainWindow
{
    Q_OBJECT

public:
    explicit Close(QWidget *parent = nullptr);
    ~Close();

private slots:
    void on_Close_app_btn_clicked();

private:
    Ui::Close *ui;
};

#endif // CLOSE_H
