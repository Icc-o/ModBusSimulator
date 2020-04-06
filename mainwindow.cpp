#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QStringList>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    w = new MainWidget(this);
    setWindowTitle("ModBus");
    setCentralWidget(w);
}

MainWindow::~MainWindow()
{
    delete ui;
}


