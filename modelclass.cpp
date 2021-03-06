#include "modelclass.h"
#include "google/protobuf/descriptor.pb.h"

#include <string.h>
#include <qdebug>

using namespace std;

namespace fantasybit_bx {
ModelClass::ModelClass()
{

}

void ModelClass::init() {
    qDebug() << BlockMeta::descriptor()->file()->DebugString().data();
    google::protobuf::FileDescriptorProto fdp;
    BlockMeta::descriptor()->file()->CopyTo(&fdp);
    for ( auto db : fdp.message_type()) {
        qDebug() << db.DebugString().data();
    }

    bc.init();
    if ( !bc.Valid() )
        qDebug() << "invalid";
    else {
        bc.SeekToFirst();
        //bc.Next();
        if ( !bc.Valid() )
            qDebug() << "invalid";
        else
            first = bc.GetCurrentBlock().signedhead().head().num();
    }

    //setProperty("blockheight",bc.GetBlockHeight());
}


qint32 ModelClass::firstblock() {
    return first;
}

qint32 ModelClass::blockheight() {
    return bc.GetBlockHeight();
}

int ModelClass::rowCount(const QModelIndex &parent) const {
    //qInfo() << " 0test " << bc.GetBlockHeight();

    return bc.GetBlockHeight();// - first;
}

QVariant ModelClass::data(const QModelIndex & index, int role) const {
    //qInfo() << " 1test " << index;
    int row = bc.GetBlockHeight() - index.row();// + 1 + first;
    if (index.isValid()) {

        bc.Seek(row);

        if (bc.Valid()) {

            if (role == BlockNum) {
                //qInfo() << " BlockNum " << bc.GetCurrentBlock().DebugString().data();

                return QVariant(bc.GetCurrentBlock().signedhead().head().num());
            }
            else if (role == CreationTime) {
                //qInfo() << " CreationTime " << bc.GetCurrentBlock().DebugString().data();

                return QVariant(bc.GetCurrentBlock().signedhead().head().timestamp());



//            bc.GetCurrentBlock().signedhead().head().


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
}
