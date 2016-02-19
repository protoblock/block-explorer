#ifndef MODELCLASS_H
#define MODELCLASS_H

#include <QObject>
#include <QAbstractListModel>
#include <QStringListModel>

#include <blockchain.h>

//class ModelClass : public QStringListModel
class ModelClass : public QAbstractItemModel
{
    Q_OBJECT
protected:
    Blockchain bc{};

public:
    ModelClass();

    enum ModelRoles {
        BlockNum = Qt::UserRole + 1
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    QModelIndex parent(const QModelIndex & index = QModelIndex()) const;
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;

    QHash<int, QByteArray> roleNames() const;
};

#endif // MODELCLASS_H
