#pragma once
#include <QObject>
#include <QStringList>
#include <QPixmap>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <vector>
#include <QLabel>
#include <QStyle>

class FileController : public QObject
{
    Q_OBJECT

public:
    explicit FileController(QMainWindow* decision, QStandardItemModel* model)
        : decision(decision), model(model){};

public slots:


private:
    QMainWindow* decision;
    QMessageBox msgBox;
    QStandardItemModel* model;

    QString recentPath;
    std::vector<QString> folderList;
};
