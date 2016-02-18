#include "modelclass.h"

ModelClass::ModelClass()
{

}

int ModelClass::rowCount(const QModelIndex &parent) const {
    return 10;
}

int ModelClass::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant ModelClass::data(const QModelIndex & index, int role) const {
    return QVariant();
}

QModelIndex ModelClass::parent(const QModelIndex & index) const {
    return index;
}

QModelIndex ModelClass::index(int row, int column, const QModelIndex & parent) const {
    return parent;
}

QHash<int, QByteArray> ModelClass::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[BlockNum] = "blocknum";
    return roles;
}
