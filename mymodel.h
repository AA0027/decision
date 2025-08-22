#pragma once

#include <QObject>
#include <QSortFilterProxyModel>

class MyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    MyModel(QObject* parent = nullptr);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};
