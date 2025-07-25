#pragma once

#include <QMainWindow>
#include <QMediaPlayer>
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
