#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Initialize UI object
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    // Destroy UI object
    delete ui;
}
