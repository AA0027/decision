#include "decision.h"

Decision::Decision(QWidget* parent) : QMainWindow(parent)
{
    decisionWidget = new QWidget(this);

    QHBoxLayout* layout = new QHBoxLayout();

    sideMenu = new SideMenu(decisionWidget);
    displayWidget = new DisplayWidget(decisionWidget);

    sideMenu ->insertWidget(displayWidget);



    QMenuBar* menuBar = new QMenuBar();
    QMenu *fileMenu = menuBar ->addMenu("파일");
    QAction *openAction = fileMenu -> addAction("열기");

    connect(openAction, &QAction::triggered, sideMenu, &SideMenu::selectFolder);

    setMenuBar(menuBar);


    layout -> addWidget(sideMenu);
    layout -> addWidget(displayWidget);

    decisionWidget -> setLayout(layout);
    setCentralWidget(decisionWidget);

}
