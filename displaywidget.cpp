#include "displaywidget.h"
#include "imageloader.h"
#include <QMessageBox>
#include <QThread>


DisplayWidget::DisplayWidget(QWidget* parent) : QWidget(parent), isPlaying(false)
{
    setupTimer();
    connect(this, &DisplayWidget::bufferIsNotReady, this, &DisplayWidget::loading);

    msgBox.styleSheet();
    QVBoxLayout* screenLayout = new QVBoxLayout(this);

    videoView = new QLabel();
    videoView -> setAlignment(Qt::AlignCenter);
    videoView -> setMinimumSize(640, 480);
    videoView ->setStyleSheet("border: 1px solid gray;");

    QHBoxLayout* bottomLayout = new QHBoxLayout();
    QHBoxLayout* controlLayout = new QHBoxLayout();

    playBtn = new QPushButton(this);
    playBtn -> setIcon(QIcon(":/play.png"));
    connect(playBtn, &QPushButton::clicked, this, &DisplayWidget::controlPlay);

    nextBtn = new QPushButton(this);
    nextBtn -> setIcon(QIcon(":/next.png"));
    connect(nextBtn, &QPushButton::clicked, this, &DisplayWidget::nextFrame);

    previousBtn = new QPushButton(this);
    previousBtn -> setIcon(QIcon(":/before.png"));
    connect(previousBtn, &QPushButton::clicked, this, &DisplayWidget::previousFrame);

    stopBtn = new QPushButton(this);
    stopBtn -> setIcon(QIcon(":/stop.png"));
    connect(stopBtn, &QPushButton::clicked, this, &DisplayWidget::stop);

    customPushBtn(playBtn);
    customPushBtn(nextBtn);
    customPushBtn(previousBtn);
    customPushBtn(stopBtn);

    controlLayout -> addWidget(previousBtn);
    controlLayout -> addWidget(playBtn);
    controlLayout -> addWidget(stopBtn);
    controlLayout -> addWidget(nextBtn);

    frameInfo = new QLabel("프레임: 0 / 0");
    frameInfo -> setStyleSheet(
        "QLabel {"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        );

    bottomLayout -> addWidget(frameInfo);
    bottomLayout->addStretch();
    bottomLayout -> addLayout(controlLayout);
    bottomLayout->addStretch();

    slider = new QSlider(Qt::Horizontal);
    slider -> setEnabled(false);
    connect(slider, &QSlider::valueChanged, this, &DisplayWidget::onSliderChanged);

    screenLayout -> addWidget(videoView, 5);
    screenLayout -> addWidget(slider, 2);
    screenLayout -> addLayout(bottomLayout);

    loadingMovie = new QMovie(":/loading.gif", QByteArray(), this);
}

void DisplayWidget::customPushBtn(QPushButton* btn)
{
    btn -> setFixedSize(30, 30);
    btn -> setStyleSheet(
        "QPushButton {"
        "    background-color: #888880"
        "}"
        "QPushButton:hover {"
        "    background-color: #cbcbc7;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #cbcbc7;"
        "}"
        );
}

//  ============== slots ===============

// 재생할 이미지들 준비
void DisplayWidget::loadImages(const QStringList &imageList)
{
    this -> imageList = imageList;
    currentFrame = 0;
    slider->setEnabled(true);
    slider->setRange(0, imageList.size() - 1);
    slider->setValue(0);


    videoView -> setMovie(loadingMovie);
    videoView -> setAlignment(Qt::AlignCenter);
    loadingMovie -> start();

    QThread* thread = new QThread;
    ImageLoader* loader = new ImageLoader(imageList, videoView);
    loader->moveToThread(thread);

    // 작업 완료 시 처리
    connect(loader, &ImageLoader::imagesLoaded, this, [=](std::vector<QPixmap> loadedImages) {
        loadingMovie->stop();
        videoView->clear();

        if(video.empty())
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("정보");
            msgBox.setText("동영상 재생 준비 완료");
            msgBox.exec();
        }

        video.insert(video.end(), loadedImages.begin(), loadedImages.end());

        if(video.size() == imageList.size())
        {
            loader->deleteLater();
            thread->quit();
            thread->deleteLater();
        }

    });

    // 스레드 시작 시 작업 수행
    connect(thread, &QThread::started, loader, &ImageLoader::loadImages);

    // 시작
    thread->start();
}

// 동영상 재생/일시정지 (slots)
void DisplayWidget::controlPlay()
{
    if(video.empty())
        return;
    if (isPlaying) {
        timer->stop();
        playBtn->setIcon(QIcon(":/play.png"));
        isPlaying = false;
    } else {
        timer->start();
        playBtn->setIcon(QIcon(":/pause.png"));
        isPlaying = true;
    }
}

// 동영상 정지
void DisplayWidget::stop()
{
    if(video.empty())
        return;

    timer->stop();
    currentFrame = 0;
    isPlaying = false;
    playBtn->setIcon(QIcon(":/play.png"));
    slider->setValue(0);
    play();
}

// 다음 프레임
void DisplayWidget::nextFrame()
{
    if(!video.empty())
    {
        if(video.size() == currentFrame + 1)
        {
            stop();
            return;
        }

        if(video.size() <= currentFrame + 1)
        {
            emit bufferIsNotReady();
            return;
        }
         // vector 에러처리필요?
        currentFrame = (currentFrame + 1);
        play();
        updateSlider();

    }
}


// 이전 프레임
void DisplayWidget::previousFrame()
{
    if (!imageList.empty()) {
        // vector 에러처리필요?
        currentFrame = (currentFrame - 1 + imageList.size());
        play();
        updateSlider();
    }
}

// slider 이벤트
void DisplayWidget::onSliderChanged(int value)
{
    if (!imageList.isEmpty() && value < imageList.size()) {
        currentFrame = value;
        play();
    }
}

// 동영상이 계속 재생되게
void DisplayWidget::play()
{

    videoView->setPixmap(video[currentFrame]);
    frameInfo->setText(
        QString("프레임: %1 / %2").arg(currentFrame + 1).arg(imageList.size())
        );

}

// 로딩표시
void DisplayWidget::loading()
{
    videoView -> setMovie(loadingMovie);
    videoView -> setAlignment(Qt::AlignCenter);
    loadingMovie -> start();
}

//  ============== slots END ===============




void DisplayWidget::setupTimer()
{
    timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer, &QTimer::timeout, this, &DisplayWidget::nextFrame);
}


void DisplayWidget::updateSlider()
{
    slider->blockSignals(true);
    slider->setValue(currentFrame);
    slider->blockSignals(false);
}
