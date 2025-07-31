#pragma once

#include <QMainWindow>
#include <QObject>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include "sidemenu.h"
#include "displaywidget.h"

class Decision : public QMainWindow
{
    Q_OBJECT
public:
    Decision(QWidget* parent = nullptr);

private:
    QWidget* decisionWidget;
    SideMenu* sideMenu;
    DisplayWidget* displayWidget;
};
