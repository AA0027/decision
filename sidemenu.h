#pragma once

#include "displaywidget.h"

#include <QObject>
#include <QStandardItemModel>
#include <QTreeView>
#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QThread>
#include <QFileSystemModel>
#include <QListWidget>
#include <QTreeWidget>
#include <QCheckBox>

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
    void colapseFolder(Qt::CheckState state);
private:
    DisplayWidget* display;
    QCheckBox* treeRoot;
    QTreeView* treeView;
    QFileSystemModel* model;
    QAction* playAction;
    QMessageBox msgBox;
    QString selectedFolder;
    QString recentPath;
    QStringList imageList;
};
