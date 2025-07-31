#include "sidemenu.h"


SideMenu::SideMenu(QWidget* parent) : QWidget(parent)
{
    setMaximumWidth(250);


    folderView = new QTreeView();
    folderModel = new QStandardItemModel(this);

    folderView -> setModel(folderModel);
    folderView -> setHeaderHidden(true);
    folderView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    folderView -> setContextMenuPolicy(Qt::CustomContextMenu);

    deleteAction = new QAction("삮제");
    connect(folderView, &QTreeView::customContextMenuRequested,
            this, &SideMenu::showContextMenu);

    connect(folderView, &QTreeView::doubleClicked,
            this, &SideMenu::preparePlay);

    connect(deleteAction, &QAction::triggered, this, &SideMenu::deleteFolder);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(folderView);
    setLayout(layout);

}

void SideMenu::insertWidget(DisplayWidget* displayWidget)
{
    display = displayWidget;
}

// ================ slots =======================
// 메뉴에서 파일 선택할떄 (slots)
void SideMenu::selectFolder()
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

    QStandardItem* rootNode = folderModel -> invisibleRootItem();

    if(folderList.size() > 4)
    {
        msgBox.setWindowTitle("경고");
        msgBox.setText("폴더추가 불가");
        msgBox.exec();

        return;
    }

    folderList.emplace_back(path);

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

// 더블클릭시 재생준비
void SideMenu::preparePlay(const QModelIndex &index)
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

        QStandardItem *item = folderModel -> itemFromIndex(index);
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

        display ->loadImages(imageList);

    }
}

// 폴더 우클릭스 컨텍스트 메뉴창 띄움 (slots)
void SideMenu::showContextMenu(const QPoint &pos)
{
    QModelIndex index = folderView->indexAt(pos);

    // 유효한 인덱스이고, 최상위 루트 아이템인지 확인
    if (index.isValid() && index.parent() == QModelIndex()) {
        QMenu contextMenu(this);
        contextMenu.addAction(deleteAction);

        // 글로벌 좌표로 변환하여 메뉴 표시
        contextMenu.exec(folderView->mapToGlobal(pos));
    }
}

// 선택됀 폴더 삭제 (slots)
void SideMenu::deleteFolder()
{
    QModelIndex currentIndex = folderView->currentIndex();

    // 최상위 루트 아이템인지 다시 한번 확인
    if (currentIndex.isValid() && currentIndex.parent() == QModelIndex()) {
        // 사용자 확인 대화상자 (선택사항)
        int ret = QMessageBox::question(this, "삭제 확인",
                                        "선택한 항목을 삭제하시겠습니까?",
                                        QMessageBox::Yes | QMessageBox::No);

        if (ret == QMessageBox::Yes) {
            // 모델에서 해당 행 삭제
            folderModel->removeRow(currentIndex.row());

            // 또는 QStandardItemModel을 사용하는 경우:
            // QStandardItem *item = viewModel->itemFromIndex(currentIndex);
            // if (item) {
            //     viewModel->removeRow(item->row());
            // }
        }
    }

}

// ================ END slots =======================
