#include "sidemenu.h"

#include <QApplication>


SideMenu::SideMenu(QWidget* parent) : QWidget(parent)
{
    setMaximumWidth(250);

    model = new QFileSystemModel();
    model->setNameFilters(QStringList() << "*.png" << "*.jpg");
    model->setNameFilterDisables(false);

    treeView = new QTreeView();
    treeView -> setEditTriggers(QAbstractItemView::NoEditTriggers);
    treeView -> setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeView, &QTreeView::customContextMenuRequested,
            this, &SideMenu::showContextMenu);


    treeRoot = new QCheckBox(this);
    treeRoot -> hide();
    // treeRoot -> setStyleSheet()
    connect(treeRoot, &QCheckBox::checkStateChanged,
            this, &SideMenu::colapseFolder);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(treeRoot);
    layout->addWidget(treeView);
    setLayout(layout);

}

void SideMenu::insertWidget(DisplayWidget* displayWidget)
{
    display = displayWidget;
}



// ================ slots =======================
void SideMenu::selectFolder()
{
    if(!treeView -> model())
    {
        treeView -> setModel(model);
        treeView -> hideColumn(1);
        treeView -> hideColumn(2);
        treeView -> hideColumn(3);
        treeView->setHeaderHidden(true);

    }

    if(treeRoot -> isHidden())
        treeRoot -> show();

    QString path;
    QString startPath = recentPath.isEmpty() ? QDir::rootPath() : recentPath;

    path = QFileDialog::getExistingDirectory(this, "폴더선택", startPath
                                             , QFileDialog::ShowDirsOnly);

    if (path.isEmpty()) {
        return;
    }

    recentPath = path;

    QDir d(path);
    treeRoot->setIcon(style()->standardIcon(QStyle::SP_DirIcon));
    treeRoot->setText(d.dirName());
    treeRoot->setChecked(false);

    QFont font = treeRoot->font();
    font.setBold(true);
    treeRoot->setFont(font);

    model -> setHeaderData(0, Qt::Horizontal, d.dirName());

    model -> setRootPath(path);

    treeView -> setRootIndex(model -> index(path));
}


void SideMenu::preparePlay()
{
    QModelIndex idx = playAction->data().value<QModelIndex>();

    if(idx.isValid() && model -> isDir(idx))
    {
        QFileInfo info = model -> fileInfo(idx);
        QString path = info.absoluteFilePath();

        QDir dir(path);

        QStringList nameFilters;
        nameFilters << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp" << "*.gif" << "*.tiff";

        imageList.clear();
        imageList = dir.entryList(nameFilters, QDir::Files, QDir::Name);

        display -> clearVideo();

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

        display ->loadImages(imageList);
    }
    else
    {
        msgBox.setWindowTitle("경고");
        msgBox.setText("유효하지 않는 폴더입니다.");
        msgBox.exec();

        return;
    }


}


void SideMenu::showContextMenu(const QPoint &pos)
{

    QModelIndex index = treeView->indexAt(pos);


    if (index.isValid())
    {
        QMenu contextMenu(this);
        playAction = new QAction("재생");
        contextMenu.addAction(playAction);
        playAction->setData(QVariant::fromValue(index));
        connect(playAction, &QAction::triggered, this, &SideMenu::preparePlay);

        // 글로벌 좌표로 변환하여 메뉴 표시
        contextMenu.exec(treeView->mapToGlobal(pos));
    }
}

void SideMenu::colapseFolder(Qt::CheckState state)
{
    if(state)
        treeView ->collapseAll();
}

// ================ END slots =======================
