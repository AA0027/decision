#include "mymodel.h"

#include <QFileSystemModel>

MyModel::MyModel(QObject* parent) : QSortFilterProxyModel(parent) {}

bool MyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    QFileSystemModel* fsm = qobject_cast<QFileSystemModel*>(sourceModel());

    if (!fsm) return true;

    QString path = fsm->filePath(index);

    // 디렉토리만 허용
    QFileInfo info(path);
    if (!info.isDir()) return false;

    return true;
}
