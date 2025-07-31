#pragma once

#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QSlider>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMovie>
#include <QMessageBox>
#include <QTimer>
#include <QObject>

class DisplayWidget : public QWidget
{
    Q_OBJECT
public:
    DisplayWidget(QWidget* parent = nullptr);
    void loadImages(const QStringList &imageList);

private slots:
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

    void loading();

signals:
    void bufferIsNotReady();

private:
    QLabel* videoView;
    QLabel* frameInfo;
    QPushButton* playBtn;
    QPushButton* stopBtn;
    QPushButton* nextBtn;
    QPushButton* previousBtn;
    QSlider* slider;
    QMovie* loadingMovie;
    QTimer* timer;

    QStringList imageList;
    std::vector<QPixmap> video;
    int currentFrame;
    bool isPlaying;

    QMessageBox msgBox;

private:
    void customPushBtn(QPushButton* btn);
    void setupTimer();
    void updateSlider();
};
