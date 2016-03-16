#ifndef MODELCLASS_H
#define MODELCLASS_H

#include <QObject>
#include <QAbstractListModel>
#include <QStringListModel>

#include <blockchain.h>

class ModelClass : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(qint32 blockheight READ blockheight)

protected:
    Blockchain bc{};
    int first;
public:
    ModelClass();
    qint32 blockheight();
    void init();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
};

#endif // MODELCLASS_H
