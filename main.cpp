// #include "mainwindow.h"

#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QFileDialog>
#include <QAbstractItemModel>
#include <QFileSystemModel>
#include <QTreeView>
#include <QMainWIndow>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    // QPushButton button("폴더 선택");
    // QObject::connect(&button, &QPushButton::clicked, [&](){
    //     QString folderPath = QFileDialog::getExistingDirectory(
    //         nullptr,
    //         "폴더 선택",
    //         "",
    //         QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    //         );

    //     if(folderPath.isEmpty())
    //     {
    //         qDebug()<<"선택한 폴더 경로: " << folderPath;
    //     }
    // }
    // );

    // button.resize(200, 50);
    // button.show();

    MainWindow w;
    w.show();
    return a.exec();
}
