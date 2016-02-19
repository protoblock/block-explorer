#include "modelclass.h"

#include <string.h>

using namespace std;

ModelClass::ModelClass()
{

}

int ModelClass::rowCount(const QModelIndex &parent) const {
    return bc.GetBlockHeight();
}

int ModelClass::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant ModelClass::data(const QModelIndex & index, int role) const {
    if (index.isValid()) {
        bc.Seek(index.row());

        if (bc.Valid()) {
            if (role == BlockNum) {
                return QVariant(bc.GetCurrentBlock().signedhead().head().num());
            }
        }
    }

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
