#include "sidemenu.h"

#include <QApplication>


SideMenu::SideMenu(QWidget* parent) : QWidget(parent)
{
    setMaximumWidth(250);

    tree = new QTreeWidget(this);
    tree -> setContextMenuPolicy(Qt::CustomContextMenu);
    tree -> setColumnCount(2);
    tree -> hideColumn(1);

    tree -> setHeaderLabel("폴더 구조");


    deleteAction = new QAction("삮제");
    connect(tree, &QTreeWidget::customContextMenuRequested,
            this, &SideMenu::showContextMenu);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(tree);
    setLayout(layout);

}

void SideMenu::insertWidget(DisplayWidget* displayWidget)
{
    display = displayWidget;
}



// ================ slots =======================
void SideMenu::selectFolder()
{
    QString path;
    QString startPath = recentPath.isEmpty() ? QDir::rootPath() : recentPath;

    path = QFileDialog::getExistingDirectory(this, "폴더선택", startPath
                                             , QFileDialog::ShowDirsOnly);

    if (path.isEmpty()) {
        return;
    }
    tree -> clear();

    recentPath = path;


    QDir dir(path);

    QDir::Filters filters = QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files | QDir::Readable;
    // filters &= ~QDir::Hidden;

    QStringList imageExtensions = {"png"};
    QStringList imgExt = {".png"};

    QFileInfoList list = dir.entryInfoList(filters, QDir::DirsFirst);

    // 파일만 다시 거르기
    QFileInfoList filtered;
    for (const QFileInfo &info : std::as_const(list)) {
        if (info.isDir() || imageExtensions.contains(info.suffix(), Qt::CaseInsensitive)) {
            filtered << info;
        }
    }


    QTreeWidgetItem* rootItem = new QTreeWidgetItem(tree);
    rootItem -> setText(0, dir.dirName());
    rootItem -> setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));
    rootItem -> setText(1, dir.absolutePath());

    for(const QFileInfo& entry : std::as_const(filtered))
    {

        if (entry.isDir()) {
            QTreeWidgetItem* item = new QTreeWidgetItem(rootItem);
            item -> setText(0, entry.fileName());
            item -> setText(1, entry.filePath());
            item -> setIcon(0, QApplication::style() -> standardIcon(QStyle::SP_DirIcon));

            QString dirPath = entry.filePath();
            QDir d(dirPath);
            QFileInfoList stateDir = d.entryInfoList(QDir::Dirs| QDir::NoDotAndDotDot);

            if(!stateDir.empty())
            {
                for(const QFileInfo& e : std::as_const(stateDir))
                {
                    QTreeWidgetItem* i = new QTreeWidgetItem(item);
                    i -> setText(0, e.fileName());
                    i -> setText(1, e.filePath());
                    i-> setIcon(0, QApplication::style() -> standardIcon(QStyle::SP_DirIcon));

                    QString subPath = e.filePath();
                    QDir subDir(subPath);
                    QFileInfoList files = subDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);


                    for(const QFileInfo& f : std::as_const(files))
                    {
                        QTreeWidgetItem* img = new QTreeWidgetItem(i);
                        img -> setText(0, e.fileName());
                        img -> setText(1, e.filePath());
                        img-> setIcon(0, QApplication::style() -> standardIcon(QStyle::SP_FileIcon));
                    }
                }
            }

        }
    }

    tree->addTopLevelItem(rootItem);
    rootItem->setExpanded(true);
}


void SideMenu::preparePlay()
{
    imageList.clear();

    QStringList allowedExtensions = {"*.png"};
    QFileInfo info(selectedFolder);
    QDir d(selectedFolder);


    if(info.isFile())
    {
        msgBox.setWindowTitle("정보");
        msgBox.setText("폴더를 선택해주세요");
        msgBox.exec();

        return;
    }

    d.setNameFilters(allowedExtensions);
    d.setFilter(QDir::Files);

    imageList = d.entryList();

    if(imageList.isEmpty())
    {
        msgBox.setWindowTitle("정보");
        msgBox.setText("이미지가 존재하지 않습니다.");
        msgBox.exec();

        return;
    }

    display -> clearVideo();

    for(int i = 0; i < imageList.size(); i++)
    {
        imageList[i] = d.absoluteFilePath(imageList[i]);
    }

    display -> loadImages(imageList);
}

// 폴더 우클릭스 컨텍스트 메뉴창 띄움 (slots)
void SideMenu::showContextMenu(const QPoint &pos)
{

    QTreeWidgetItem* item = tree -> itemAt(pos);

    if(!item)
        return;

    selectedFolder = item->text(1);

    QMenu contextMenu(this);

    QAction* playAction = contextMenu.addAction("재생");

    connect(playAction, &QAction::triggered, this, &SideMenu::preparePlay);
    contextMenu.exec(tree -> mapToGlobal(pos));
}

// ================ END slots =======================
