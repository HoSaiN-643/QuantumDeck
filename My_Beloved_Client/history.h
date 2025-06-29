#ifndef HISTORY_H
#define HISTORY_H

#include <QMainWindow>

namespace Ui {
class History;
}

class History : public QMainWindow
{
    Q_OBJECT

public:
    explicit History(QWidget *parent = nullptr);
    ~History();

private slots:
    void onClearButtonClicked();  // اسلات برای پاک کردن تاریخچه

private:
    Ui::History *ui;
};

#endif // HISTORY_H