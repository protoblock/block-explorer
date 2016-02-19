#include "modelclass.h"

#include <string.h>
#include <qdebug>

using namespace std;

ModelClass::ModelClass()
{

}

int ModelClass::rowCount(const QModelIndex &parent) const {
    return bc.GetBlockHeight();
}

QVariant ModelClass::data(const QModelIndex & index, int role) const {
    if (index.isValid()) {
        bc.Seek(index.row() + 1);

        if (bc.Valid()) {
            if (role == BlockNum) {
                return QVariant(bc.GetCurrentBlock().signedhead().head().num());
            }
            else if (role == CreationTime) {
                return QVariant(bc.GetCurrentBlock().signedhead().head().timestamp());
            }
        }
    }

    return QVariant();
}

QHash<int, QByteArray> ModelClass::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[BlockNum] = "blocknum";
    roles[CreationTime] = "time";
    return roles;
}
