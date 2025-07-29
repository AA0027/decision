#include "mainwindow.h"

#include <QMessageBox>
#include <QTimer>


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
    viewModel(new QStandardItemModel(this)), isPlaying(false)
{

    createMenuBar();
    createUI();
}

// slots
// 폴더 열기 선택시
void MainWindow::selectFolder()
{
    QString path;
    if(recentPath == "")
    {
        path = QFileDialog::getExistingDirectory(this, "select",
                                                       QDir::homePath(), QFileDialog::ShowDirsOnly);
        recentPath = path;
    }
    else
    {
        path = QFileDialog::getExistingDirectory(this, "select",
                                                       recentPath, QFileDialog::ShowDirsOnly);
        recentPath = path;
    }

    QStandardItem* rootNode = viewModel -> invisibleRootItem();

    if(folderList.size() > 4)
    {
        QMessageBox::warning(this, "경고", "폴더추가 불가");
        return;
    }

    folderList.append(path);

    QDir dir(path);
    QIcon folderIcon = style() -> standardIcon(QStyle::SP_DirIcon);
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
        subFiles.emplaceBack(new QStandardItem(imageFiles[i]));
    }

    subFiles.emplaceBack(new QStandardItem("..."));

    rootNode -> appendRow(includeFolder);
    includeFolder -> appendColumn(subFiles);

}


// 재생할 폴더 선택
void MainWindow::preparePlay(const QModelIndex &index)
{
    if(!index.isValid())
    {
        QMessageBox::warning(this, "경고", "유효하지 않는 폴더입니다.");
        return;
    }

    QStandardItem *item = viewModel -> itemFromIndex(index);
    QString path = item -> text();
    for(const QString& p : folderList)
    {
        if(p.contains(path))
        {
            path = p;
        }
    }

    playFolder = path;
}

// 동영상 재생/일시정지
void MainWindow::controlPlay()
{

}

// 다음 프레임
void MainWindow::nextFrame()
{
    if(!imageList.isEmpty())
    {
        currentFrame = (currentFrame + 1) % imageList.size();
        updateFrame();
        // updateSlider();
    }
}

// END slots



void MainWindow::createMenuBar()
{
    QMenuBar* menuBar = new QMenuBar(this);

    QMenu *fileMenu = menuBar ->addMenu("파일");

    QAction *openAction = fileMenu -> addAction("열기");
    connect(openAction, &QAction::triggered, this, &MainWindow::selectFolder);

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


    layout -> addWidget(treeView, 1);
    layout -> addWidget(screenWidget, 5);


    central -> setLayout(layout);
    setCentralWidget(central);
}

void MainWindow::fileViewer()
{


    treeView = new QTreeView(central);

    treeView -> setModel(viewModel);
    treeView -> setHeaderHidden(true);

    connect(treeView, &QTreeView::doubleClicked, this, &MainWindow::preparePlay);
}

void MainWindow::screenViewer()
{
    screenWidget = new QWidget(central);

    QVBoxLayout* screenLayout = new QVBoxLayout(screenWidget);

    videoView = new QLabel();
    videoView -> setAlignment(Qt::AlignCenter);
    videoView -> setMinimumSize(640, 480);
    videoView ->setStyleSheet("border: 1px solid gray;");


    QHBoxLayout* controlLayout = new QHBoxLayout();
    playBtn = new QPushButton();
    playBtn -> setIcon(QIcon(":/play.png"));
    // connect(playBtn, &QPushButton::clicked, this, &MainWindow::controlPlay);

    nextBtn = new QPushButton();
    nextBtn -> setIcon(QIcon(":/next.png"));


    beforeBtn = new QPushButton();
    beforeBtn -> setIcon(QIcon(":/before.png"));

    customPushBtn(playBtn);
    customPushBtn(nextBtn);
    customPushBtn(beforeBtn);

    controlLayout -> addWidget(beforeBtn);
    controlLayout -> addWidget(playBtn);
    controlLayout -> addWidget(nextBtn);

    controlLayout -> setAlignment(Qt::AlignCenter);

    slider = new QSlider(Qt::Horizontal);
    slider -> setEnabled(false);

    screenLayout -> addWidget(videoView, 5);
    screenLayout -> addWidget(slider, 2);
    screenLayout -> addLayout(controlLayout);
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

// =====================================================

void MainWindow::setupTimer()
{
    timer = new QTimer(this);
    timer->setInterval(10);
    connect(timer, &QTimer::timeout, this, &MainWindow::nextFrame);
}

void MainWindow::loadImages()
{

}

void MainWindow::updateFrame()
{

}

void MainWindow::updateSlider()
{

}














