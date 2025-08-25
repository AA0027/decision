#pragma once

#include "displaywidget.h"

#include <QObject>
#include <QStandardItemModel>
#include <QTreeView>
#include <QWidget>
#include <vector>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QThread>
#include <QFileSystemModel>
#include <QListWidget>
#include <QTreeWidget>

#include "displaywidget.h"

class SideMenu : public QWidget
{
    Q_OBJECT
public:
    SideMenu(QWidget* parent = nullptr);
    void insertWidget(DisplayWidget* displayWidget);

public slots:
    void selectFolder();
    void preparePlay();

private slots:
    void showContextMenu(const QPoint &pos);
private:
    DisplayWidget* display;

    QTreeWidget* tree;

    QMessageBox msgBox;
    QAction* deleteAction;
    QString selectedFolder;
    QString recentPath;
    QStringList imageList;
};
