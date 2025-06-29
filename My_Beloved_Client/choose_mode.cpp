#include "choose_mode.h"
#include "ui_choose_mode.h"

// Constructor: Initializes the main window and sets up the UI
Choose_mode::Choose_mode(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Choose_mode)
{
    ui->setupUi(this);
    
    // Connect buttons to slots
    connect(ui->fourPlayerButton, &QPushButton::clicked, this, &Choose_mode::onFourPlayerClicked);
    connect(ui->twoPlayerButton, &QPushButton::clicked, this, &Choose_mode::onTwoPlayerClicked);
    connect(ui->exitButton, &QPushButton::clicked, this, &Choose_mode::onExitClicked);
}

// Destructor: Cleans up the UI
Choose_mode::~Choose_mode()
{
    delete ui;
}

// Slot for 4 Player button click
void Choose_mode::onFourPlayerClicked()
{
    // Implement 4 player online mode logic
}

// Slot for 2 Player button click
void Choose_mode::onTwoPlayerClicked()
{
    // Implement 2 player online mode logic
}

// Slot for Exit button click
void Choose_mode::onExitClicked()
{
    close(); // Close the application
}