#include "mainwindow.h"

#include <QSplitter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QSplitter* spliter = new QSplitter(this);
}

MainWindow::~MainWindow() {}
