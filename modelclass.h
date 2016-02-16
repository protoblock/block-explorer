#ifndef MODELCLASS_H
#define MODELCLASS_H

#include <QObject>
#include <QAbstractListModel>
#include <QStringListModel>


class ModelClass : public QStringListModel
{
    Q_OBJECT
public:
    ModelClass();
};

#endif // MODELCLASS_H
