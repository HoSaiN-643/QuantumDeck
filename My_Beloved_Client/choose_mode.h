#ifndef CHOOSE_MODE_H
#define CHOOSE_MODE_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Choose_mode; }
QT_END_NAMESPACE

// Main window class for online mode selection
class Choose_mode : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor
    Choose_mode(QWidget *parent = nullptr);
    // Destructor
    ~Choose_mode();

private slots:
    // Slot for 4 player mode button
    void onFourPlayerClicked();
    // Slot for 2 player mode button
    void onTwoPlayerClicked();
    // Slot for exit button
    void onExitClicked();

private:
    Ui::Choose_mode *ui; // UI pointer
};

#endif // CHOOSE_MODE_H