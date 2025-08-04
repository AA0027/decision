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

#include "displaywidget.h"

class SideMenu : public QWidget
{
    Q_OBJECT
public:
    SideMenu(QWidget* parent = nullptr);
    void insertWidget(DisplayWidget* displayWidget);

public slots:
    void selectFolder();
    void preparePlay(const QModelIndex &index);

private slots:
    void showContextMenu(const QPoint &pos);
    void deleteFolder();
private:
    DisplayWidget* display;
    QTreeView* folderView;
    QStandardItemModel* folderModel;
    QMessageBox msgBox;
    QAction* deleteAction;
    QString currentFolder;
    QString recentPath;
    std::vector<QString> folderList;
    QStringList imageList;
};
