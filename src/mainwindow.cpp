#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "emitter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
    emit Emitter::ptr()->terminate();
    return QMainWindow::closeEvent(ev);
}

