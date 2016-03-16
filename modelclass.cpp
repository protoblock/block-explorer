#include "modelclass.h"

#include <string.h>
#include <qdebug>

#include "blockdisplay.h"

using namespace std;

ModelClass::ModelClass()
{

}

void ModelClass::init() {
    bc.init();
    if ( !bc.Valid() )
        qDebug() << "invalid";
    else {
        bc.SeekToFirst();
        bc.Next();
        if ( !bc.Valid() )
            qDebug() << "invalid";
        else
            first = bc.GetCurrentBlock().signedhead().head().num();
    }

    //setProperty("blockheight",bc.GetBlockHeight());
}

qint32 ModelClass::blockheight() {
    return bc.GetBlockHeight();
}

int ModelClass::rowCount(const QModelIndex &parent) const {
    //qInfo() << " 0test " << bc.GetBlockHeight();

    return bc.GetBlockHeight() - first;
}

QVariant ModelClass::data(const QModelIndex & index, int role) const {
    qInfo() << " 1test " << index;
    int row = bc.GetBlockHeight() - index.row();// + 1 + first;
    if (index.isValid()) {

        bc.Seek(row);

        if (bc.Valid()) {
            if (role == Qt::DisplayRole) {
                int blocknum = bc.GetCurrentBlock().signedhead().head().num();
                long timestamp = bc.GetCurrentBlock().signedhead().head().timestamp();

                BlockDisplay dsply{blocknum, timestamp};
                QVariant var = QVariant::fromValue(dsply);
                return var;
            }
        }
    }

    return QVariant();
}
