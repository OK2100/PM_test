#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    h = new RawTDChistos(this);
    h->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

