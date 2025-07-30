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
#include <QMessageBox>
#include <QResizeEvent>
#include <QMovie>
#include <QTimer>
#include <QThread>

#include "imageloader.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);


private slots:
    // 폴더 추가
    void selectFolder();

    // 폴더 설정 메뉴 보여주기
    void showContextMenu(const QPoint &pos);

    // 산텍한 폴더 삭제
    void deleteFolder();

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

    // slider 이벤트
    void onSliderChanged(int value);

    void play();


private:
    // window의 center widget
    QWidget* central;

    // 파일 트리 보여주는 widget (parent : central)
    QWidget* fileWidget;
    QAction* deleteAction;
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
    QPushButton* stopBtn;
    QPushButton* previousBtn;
    QSlider* slider;

    QString recentPath;
    QList<QString> folderList;


    // 동영상 재생관련 변수
    QTimer* timer;

    QStringList imageList;
    QLabel* frameInfo;

    std::vector<QPixmap> video;
    QMovie* loadingMovie;
    bool isPlaying;
    int currentFrame;

    // 기타
    QMessageBox msgBox;

private:
    void createUI();
    void createMenuBar();
    void fileViewer();
    void screenViewer();
    void customPushBtn(QPushButton* btn);

    void setupTimer();
    void updateFrame();
    void updateSlider();



    void resizeEvent(QResizeEvent* event) override
    {
        if(!imageList.isEmpty())
        {
            videoView -> setMovie(loadingMovie);
            videoView -> setAlignment(Qt::AlignCenter);
            loadingMovie -> start();

            QThread* thread = new QThread;
            ImageLoader* loader = new ImageLoader(imageList, videoView);
            loader->moveToThread(thread);

            // 작업 완료 시 처리
            connect(loader, &ImageLoader::imagesLoaded, this, [=](const std::vector<QPixmap>& loadedImages) {
                loadingMovie->stop();
                videoView->clear();
                video = loadedImages;


                loader->deleteLater();
                thread->quit();
                thread->deleteLater();
            });

            // 스레드 시작 시 작업 수행
            connect(thread, &QThread::started, loader, &ImageLoader::loadImages);

            // 시작
            thread->start();
        }

        QMainWindow::resizeEvent(event);
    }
};
