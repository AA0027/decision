#pragma once

#include <QObject>
#include <QWidget>
#include <QTreeView>

class Menu_Container : public QWidget
{
    Q_OBJECT
public:
    Menu_Container(QWidget* parent = nullptr);

private:


    QTreeView* dir_list;
};
