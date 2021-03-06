#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QtQml>

#include <string>
#include <createmeta.h>
#include <ldbwriter.h>
#include "blockexplorer.h"
#include "blockchain.h"

//#include "currentheaderblock.h"

using namespace std;
using namespace fantasybit_bx;
int main(int argc, char *argv[])
{

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;


    Blockchain bc{};
    bc.init();

    BlockExplorer bx{};

    auto bh = bc.GetBlockHeight();
    int start = bx.init("blockhead");
    for ( int i = start+1 ; i < bh; i++) {
        Block b = bc.GetBlock(i);
        bx.pblock(b);
    }
    bx.endit();

    /*


    LdbWriter ldb;
    ldb.init();


    CreateMeta cm(ldb);

    Blockchain bc{};
    bc.init();
    //auto start = 0;
    BlockMeta bm{};
    auto headid = ldb.read("blocknum_2187");
    ldb.read(headid,bm);
    auto start= bm.blocknum();
    auto bh = bc.GetBlockHeight();
    string prev = headid;
    for ( int i = start+1 ; i < bh; i++) {
        Block b = bc.GetBlock(i);
        if ( i == 1199 )
            qDebug() << "";
         prev = cm.DoMeta(b,prev);
         ldb.write("blocknum_" + to_string(b.signedhead().head().num()),prev);

    }
    ldb.write("blockhead",prev);

    */
    return 0;

/*
    auto headid = ldb.read("blockhead");
    auto head = ldb.read(headid);//"3208de3df09c9617afd259fe283c9ff0166c8f54c05defc08661b1a62e43cac5");
    BlockMeta bm;
    bm.ParseFromString(head);
    auto usenext = headid;
    while ( bm.has_prev() && bm.prev() != "" ) {
        auto bprev = bm.prev();
        auto s = ldb.read(bprev);
        bm.ParseFromString(s);
        bm.set_next(usenext);
        ldb.write(bprev,bm.SerializeAsString());
        usenext = bprev;
    }

    //ldb.write("blockhead","3208de3df09c9617afd259fe283c9ff0166c8f54c05defc08661b1a62e43cac5");
    //ModelClass mc{};
    //mc.init();

    //engine.rootContext()->setContextProperty("mc", &mc);

    auto headid = ldb.read("blockhead");
    auto usenext = headid;
    while (usenext != "") {
        auto s = ldb.read(usenext);
        BlockMeta bm;
        bm.ParseFromString(s);
        usenext = bm.prev();
        qDebug() << bm.DebugString().data();
    }
*/

    /*
    auto blockmetaid = ldb.read("blockhead");
    //auto prev = ldb.read(headid);
    do {
        auto blockmetastr = ldb.read(blockmetaid);
        BlockMeta bm;
        bm.ParseFromString(blockmetastr);
        blockmetaid = bm.prev();
        qDebug() << bm.DebugString().data();
        auto trstr = ldb.read(bm.trmetaid());
        TrMeta tr;
        tr.ParseFromString(trstr);
    } while ( blockmetaid != "");
*/

//    qmlRegisterUncreatableType<ModelClass>("satoshifantasy.com",1,1,"ModelClass","modelclass singleton");

    //qmlRegisterSingletonType<CurrentHeaderBlock>("BlockExplorer",1,0,"CurrentFocusedBlock", currentHeaderBlockSingle);



    engine.load(QUrl(QStringLiteral("main.qml")));

    return app.exec();

}

