#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
    viewModel(new QStandardItemModel(this)), isPlaying(false)
{

    createMenuBar();
    createUI();
    setupTimer();

    msgBox.setStyleSheet(
        "QMessageBox {"
        "    min-width: 400px;"
        "    min-height: 200px;"
        "}"
        "QMessageBox QLabel {"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    color: #333333;"
        "    qproperty-alignment: AlignCenter;"  // 가운데 정렬
        "    min-width: 300px;"
        "}"
        );

    loadingMovie = new QMovie(":/loading.gif", QByteArray(), this);

}

// slots
// 폴더 열기 선택시 (slots)
void MainWindow::selectFolder()
{
    QString path;
    if(recentPath == "")
    {
        path = QFileDialog::getExistingDirectory(this, "select",
                                                       QDir::homePath(), QFileDialog::ShowDirsOnly);
        if (path.isEmpty()) {
            return;
        }
        recentPath = path;
    }
    else
    {
        path = QFileDialog::getExistingDirectory(this, "select",
                                                       recentPath, QFileDialog::ShowDirsOnly);
        if (path.isEmpty()) {
            return;
        }
        recentPath = path;
    }

    QStandardItem* rootNode = viewModel -> invisibleRootItem();

    if(folderList.size() > 4)
    {
        msgBox.setWindowTitle("경고");
        msgBox.setText("폴더추가 불가");
        msgBox.exec();

        return;
    }

    folderList.append(path);

    QDir dir(path);
    QIcon folderIcon = style() -> standardIcon(QStyle::SP_DirIcon);
    QIcon imageIcon = style() -> standardIcon(QStyle::SP_FileIcon);
    QStringList imageFiles;
    QStringList filters;
    filters << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp" << "*.gif";



    dir.setNameFilters(filters);
    dir.setFilter(QDir::Files | QDir::Readable | QDir::NoDotAndDotDot);
    imageFiles = dir.entryList();

    QStandardItem* includeFolder = new QStandardItem(folderIcon, dir.dirName());
    QList<QStandardItem*> subFiles;


    for(int i = 0; i < 5; i++)
    {
        subFiles.emplaceBack(new QStandardItem(imageIcon, imageFiles[i]));
    }

    subFiles.emplaceBack(new QStandardItem("..."));

    rootNode -> appendRow(includeFolder);
    includeFolder -> appendColumn(subFiles);

    msgBox.setWindowTitle("정보");
    msgBox.setText(QString("총 %1개의 이미지를 불러왔습니다.").arg(imageFiles.size()));

    msgBox.exec();
}


// 재생할 폴더 선택 (slots)
void MainWindow::preparePlay(const QModelIndex &index)
{
    if(!index.parent().isValid())
    {
        if(!index.isValid())
        {
            msgBox.setWindowTitle("경고");
            msgBox.setText("유효하지 않는 폴더입니다.");
            msgBox.exec();

            return;
        }

        QStandardItem *item = viewModel -> itemFromIndex(index);
        QString path = item -> text();

        for(int i = 0; i < folderList.size(); i++)
        {
            if(folderList[i].contains(path))
                path = folderList[i];
        }

        QDir dir(path);

        QStringList nameFilters;
        nameFilters << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp" << "*.gif" << "*.tiff";

        imageList.clear();
        imageList = dir.entryList(nameFilters, QDir::Files, QDir::Name);

        if (imageList.isEmpty()) {
            msgBox.setWindowTitle("정보");
            msgBox.setText("선택한 폴더에 이미지 파일이 없습니다.");
            msgBox.exec();

            return;
        }

        // 전체 경로로 변환
        for (int i = 0; i < imageList.size(); ++i) {
            imageList[i] = dir.absoluteFilePath(imageList[i]);
        }

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
        connect(loader, &ImageLoader::imagesLoaded, this, [=](const std::vector<QPixmap>& loadedImages) {
            loadingMovie->stop();
            videoView->clear();
            video = loadedImages;

            QMessageBox msgBox;
            msgBox.setWindowTitle("정보");
            msgBox.setText("동영상 재생 준비 완료");
            msgBox.exec();

            loader->deleteLater();
            thread->quit();
            thread->deleteLater();
        });

        // 스레드 시작 시 작업 수행
        connect(thread, &QThread::started, loader, &ImageLoader::loadImages);

        // 시작
        thread->start();

        // for(int i = 0; i < imageList.size(); i++)
        // {
        //     QPixmap pixmap(imageList[i]);
        //     if (!pixmap.isNull()) {
        //         // 이미지 레이블 크기에 맞게 스케일링
        //         video.emplace_back(pixmap);
        //     }
        // }

        // loadingMovie->stop();
        // videoView->clear();

        // msgBox.setWindowTitle("정보");
        // msgBox.setText("동영상 재생 준비 완료");
        // msgBox.exec();

    }
}

void MainWindow::play()
{
    QPixmap frame = video[currentFrame].scaled(
        videoView->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );
    videoView->setPixmap(frame);
    frameInfo->setText(
        QString("프레임: %1 / %2").arg(currentFrame + 1).arg(video.size())
        );
}

// 컨텍스트 메뉴 뜨우기 (slots)
void MainWindow::showContextMenu(const QPoint &pos)
{
    QModelIndex index = treeView->indexAt(pos);

    // 유효한 인덱스이고, 최상위 루트 아이템인지 확인
    if (index.isValid() && index.parent() == QModelIndex()) {
        QMenu contextMenu(this);
        contextMenu.addAction(deleteAction);

        // 글로벌 좌표로 변환하여 메뉴 표시
        contextMenu.exec(treeView->mapToGlobal(pos));
    }
}

// 선택됀 폴더 삭제 (slots)
void MainWindow::deleteFolder()
{
    QModelIndex currentIndex = treeView->currentIndex();

    // 최상위 루트 아이템인지 다시 한번 확인
    if (currentIndex.isValid() && currentIndex.parent() == QModelIndex()) {
        // 사용자 확인 대화상자 (선택사항)
        int ret = QMessageBox::question(this, "삭제 확인",
                                        "선택한 항목을 삭제하시겠습니까?",
                                        QMessageBox::Yes | QMessageBox::No);

        if (ret == QMessageBox::Yes) {
            // 모델에서 해당 행 삭제
            viewModel->removeRow(currentIndex.row());

            // 또는 QStandardItemModel을 사용하는 경우:
            // QStandardItem *item = viewModel->itemFromIndex(currentIndex);
            // if (item) {
            //     viewModel->removeRow(item->row());
            // }
        }
    }

}
// 동영상 재생/일시정지 (slots)
void MainWindow::controlPlay()
{
    if (imageList.isEmpty()) {
        msgBox.setWindowTitle("경고");
        msgBox.setText("먼저 이미지 폴더를 선택해주세요.");
        msgBox.exec();
        return;
    }

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

// 동영상 정지 (slot)
void MainWindow::stop()
{
    timer->stop();
    currentFrame = 0;
    isPlaying = false;
    playBtn->setIcon(QIcon(":/play.png"));
    slider->setValue(0);
    updateFrame();
}

// 다음 프레임 (slots)
void MainWindow::nextFrame()
{
    if(!video.empty())
    {
        currentFrame = (currentFrame + 1) % video.size();
        play();
        updateSlider();
    }
}

// 다음 프레임 (slots)
void MainWindow::previousFrame()
{
    if (!imageList.isEmpty()) {
        currentFrame = (currentFrame - 1 + imageList.size()) % imageList.size();
        updateFrame();
        updateSlider();
    }
}

// slider 이벤트 (slots)
void MainWindow::onSliderChanged(int value)
{
    if (!imageList.isEmpty() && value < imageList.size()) {
        currentFrame = value;
        updateFrame();
    }
}

// END slots


// UI 설정 함수들
void MainWindow::createMenuBar()
{
    QMenuBar* menuBar = new QMenuBar(this);

    QMenu *fileMenu = menuBar ->addMenu("파일");

    QAction *openAction = fileMenu -> addAction("열기");
    connect(openAction, &QAction::triggered, this, &MainWindow::selectFolder);

    fileMenu->addSeparator();

    QAction* setAction = fileMenu -> addAction("설정");
    connect(setAction, &QAction::triggered, this, &MainWindow::selectFolder);
    setMenuBar(menuBar);
}

void MainWindow::createUI()
{
    central = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout();


    fileViewer();
    screenViewer();


    layout -> addWidget(treeView);
    layout -> addWidget(screenWidget);


    central -> setLayout(layout);
    setCentralWidget(central);
}

void MainWindow::fileViewer()
{


    treeView = new QTreeView(central);
    treeView -> setMinimumWidth(200);
    treeView -> setMaximumWidth(200);
    treeView -> setModel(viewModel);
    treeView -> setHeaderHidden(true);

    treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeView, &QTreeView::customContextMenuRequested,
            this, &MainWindow::showContextMenu);
    connect(treeView, &QTreeView::doubleClicked, this, &MainWindow::preparePlay);

    deleteAction = new QAction("삭제",this);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::deleteFolder);
}

void MainWindow::screenViewer()
{
    screenWidget = new QWidget(central);

    QVBoxLayout* screenLayout = new QVBoxLayout(screenWidget);

    videoView = new QLabel();
    videoView -> setAlignment(Qt::AlignCenter);
    videoView -> setMinimumSize(640, 480);
    videoView ->setStyleSheet("border: 1px solid gray;");
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    QHBoxLayout* controlLayout = new QHBoxLayout();

    frameInfo = new QLabel("프레임: 0 / 0");
    frameInfo -> setStyleSheet(
        "QLabel {"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        );

    playBtn = new QPushButton();
    playBtn -> setIcon(QIcon(":/play.png"));
    connect(playBtn, &QPushButton::clicked, this, &MainWindow::controlPlay);

    nextBtn = new QPushButton();
    nextBtn -> setIcon(QIcon(":/next.png"));
    connect(nextBtn, &QPushButton::clicked, this, &MainWindow::nextFrame);

    previousBtn = new QPushButton();
    previousBtn -> setIcon(QIcon(":/before.png"));
    connect(previousBtn, &QPushButton::clicked, this, &MainWindow::previousFrame);

    stopBtn = new QPushButton();
    stopBtn -> setIcon(QIcon(":/stop.png"));
    connect(stopBtn, &QPushButton::clicked, this, &MainWindow::stop);

    customPushBtn(playBtn);
    customPushBtn(nextBtn);
    customPushBtn(previousBtn);
    customPushBtn(stopBtn);


    controlLayout -> addWidget(previousBtn);
    controlLayout -> addWidget(playBtn);
    controlLayout -> addWidget(stopBtn);
    controlLayout -> addWidget(nextBtn);

    // controlLayout -> setAlignment(Qt::AlignCenter);

    bottomLayout -> addWidget(frameInfo);
    bottomLayout->addStretch();
    bottomLayout -> addLayout(controlLayout);
    bottomLayout->addStretch();

    slider = new QSlider(Qt::Horizontal);
    slider -> setEnabled(false);
    connect(slider, &QSlider::valueChanged, this, &MainWindow::onSliderChanged);

    screenLayout -> addWidget(videoView, 5);
    screenLayout -> addWidget(slider, 2);
    screenLayout -> addLayout(bottomLayout);

}

void MainWindow::customPushBtn(QPushButton* btn)
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

// ======================== UI 함수설정 END =============================

void MainWindow::setupTimer()
{
    timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer, &QTimer::timeout, this, &MainWindow::nextFrame);
}


void MainWindow::updateFrame()
{

    if (!imageList.isEmpty() && currentFrame < imageList.size()) {
        QPixmap pixmap(imageList[currentFrame]);
        if (!pixmap.isNull()) {
            // 이미지 레이블 크기에 맞게 스케일링
            QPixmap scaledPixmap = pixmap.scaled(
                videoView->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                );
            videoView->setPixmap(scaledPixmap);
        }

        frameInfo->setText(
            QString("프레임: %1 / %2").arg(currentFrame + 1).arg(imageList.size())
            );
    }
}

void MainWindow::updateSlider()
{
    slider->blockSignals(true);
    slider->setValue(currentFrame);
    slider->blockSignals(false);
}


