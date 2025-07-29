#pragma once

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTreeView>
#include <QFileDialog>
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTreeView>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QStandardItemModel>
#include <QtMultimediaWidgets/qvideowidget.h>


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    // 폴더 추가
    void selectFolder();

    // 재생할 폴더 선택
    void preparePlay(const QModelIndex &index);

    // 동영상 재생/정지
    void controlPlay();

    // 동영상 정지
    void stop();

    // 다음 프레임
    void nextFrame();

    // 이전 프레임
    void previousFrame();




private:
    // window의 center widget
    QWidget* central;

    // 파일 트리 보여주는 widget (parent : central)
    QWidget* fileWidget;

    // 비디오 재생 및 조작 widget (parent : central)
    QWidget* screenWidget;

    // fileWidget에서 사용하는 widget들
    QStandardItemModel* viewModel;
    QTreeView* treeView;
    QFileDialog* fileDialog;

    // screenWidget에서 사용하는 widget들
    QLabel* videoView;
    QPushButton* playBtn;
    QPushButton* nextBtn;
    QPushButton* beforeBtn;
    QSlider* slider;

    QString recentPath;
    QList<QString> folderList;


    // 동영상 재생관련 변수
    QTimer* timer;

    QString playFolder;
    QStringList imageList;

    bool isPlaying;
    int currentFrame;

private:
    void createUI();
    void createMenuBar();
    void fileViewer();
    void screenViewer();
    void customPushBtn(QPushButton* btn);

    void setupTimer();
    void loadImages();
    void updateFrame();
    void updateSlider();
};
