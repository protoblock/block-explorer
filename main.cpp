#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QtQml>

#include <display.h>
#include <modelclass.h>
#include <actions.h>
#include <string>
#include <createmeta.h>

#include "currentheaderblock.h"
using namespace std;

/*
Blockchain
Protocol
Mutable
Operator
Command
Action
 */





static QObject *currentHeaderBlockSingle(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    CurrentHeaderBlock *example = new CurrentHeaderBlock();
    return example;
}
















using namespace fantasybit;
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    //Display dsply;

    //engine.rootContext()->setContextProperty("Display", &dsply);


/*
    //QStringList qstrl;
    LdbWriter ld;
    ld.init();
    auto bmstrid = ld.read("blockhead");

    string prev = bmstrid;

    do {
        auto bmstr = ld.read(prev);
        BlockMeta bm{};
        bm.ParseFromString(bmstr);
        qDebug() << bm.DebugString().data();

        auto tr = ld.read(bm.trmetaid());
        //auto tr = ld.read(trsid);
        TrMeta trm{};
        trm.ParseFromString(tr);
        qDebug() << trm.DebugString().data();

        MerkleTree mt;
        tr = ld.read(trm.gamemetaroot());
        mt.ParseFromString(tr);
        qDebug() << mt.DebugString().data();
        for ( auto s : mt.leaves()) {
            GameMeta dm;
            tr = ld.read(s);
            dm.ParseFromString(tr);
            qDebug() << dm.DebugString().data();

        }


        tr = ld.read(trm.datametaroot());
        mt.ParseFromString(tr);
        qDebug() << mt.DebugString().data();

        for ( auto s : mt.leaves()) {
            DataMeta dm;
            tr = ld.read(s);
            dm.ParseFromString(tr);
            if ( dm.data().type() != Data::MESSAGE)
                qDebug() << dm.DebugString().data();

        }

        tr = ld.read(bm.txmetaroot());
        mt.ParseFromString(tr);
        //qDebug() << mt.DebugString().data();

        for ( auto s : mt.leaves()) {
            TxMeta txmr{};
            tr = ld.read(s);
            txmr.ParseFromString(tr);
            qDebug() << txmr.DebugString().data();
        }

        prev = bm.prev();
    } while ( prev != "");
    return 0;
    CreateMeta cm(ld);

    {
    Blockchain bc{};
    bc.init();
    //bc.GetBlock(10);
    //bc.SeekToLast();
    bc.SeekToFirst();
    string prev = "";
    while (bc.Valid()) {

       //string ss{ bc.GetCurrentBlock().signedhead().DebugString().data() };
       //qstrl.append(ss);
       auto mr = Actions::CreateMerkle(bc.GetCurrentBlock());

       prev = cm.DoMeta(bc.GetCurrentBlock(),prev);
       bc.Next();
    };
    ld.write("blockhead",prev);
    return 0;
    //mc.setStringList(qstrl);
    }

*/
    //bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole)

    ModelClass mc{};
    mc.init();
    //Blockchain bc{};

    //Blockchain bc{};

    /*
    for (int i = 0; i < 10; ++i) {

        string ss{ bc.GetBlock(i).signedhead().DebugString().data() };
        QString s {"index"};
        QModelIndex mi {};
        QVariant v {s};
        //mc.setData(mi.child(i, 1), v, mc.BlockNum);
    }
    */

    engine.rootContext()->setContextProperty("mc", &mc);

    // 2016-02-18 - RTR commented out this line because I don't think it's needed
    qmlRegisterUncreatableType<ModelClass>("satoshifantasy.com",1,1,"ModelClass","modelclass singleton");

    qmlRegisterSingletonType<CurrentHeaderBlock>("BlockExplorer",1,0,"CurrentFocusedBlock", currentHeaderBlockSingle);



    engine.load(QUrl(QStringLiteral("main.qml")));

    /*
    QObject *item = engine.rootObjects().at(0);

    QObject::connect(item, SIGNAL(nextPressed()),
                     &dsply, SLOT(nextPressedSlot()));
    QObject::connect(item, SIGNAL(prevPressed()),
                     &dsply, SLOT(prevPressedSlot()));

    */

    return app.exec();

    /*
    QCoreApplication a(argc, argv);

    Blockchain::Test();

    Actions::Test();

    return a.exec();
    */
}

/*

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    cout << "*** init" << endl;

    Int32Comparator *cmp = new Int32Comparator();
    leveldb::Options optionsInt;
    optionsInt.create_if_missing = true;
    optionsInt.error_if_exists = false;
    optionsInt.comparator = cmp;

    leveldb::Status status;

    leveldb::DB *db;
    status = leveldb::DB::Open(optionsInt, "/Users/satoshi/ryan_work/block/blockchain", &db);
    if (!status.ok()) {
        cout << "!!! error opening db block/blockchain" << endl;
        cout << "!!! " << status.ToString() << endl;
        return 0;
    }

    leveldb::DB *dest;
    status = leveldb::DB::Open(optionsInt, "/Users/satoshi/ryan_work/block/blockchain_2", &dest);
    if (!status.ok()) {
        cout << "!!! error opening dest block/blockchain" << endl;
        cout << "!!! " << status.ToString() << endl;
        return 0;
    }

    // Print out block
    //displayBlock(db, 381);

    // Print out signed headers
    //displayHeaders(dest);

    // Print out timestamps
    displayTimestamps(db);

    // Print Diagnostics
    //displayDiagnostics(db);

    //blockchain::test(db);

    //blockchain::new_blockchain(db, dest);

    //cout << "*** Verifying Blocks" << endl;
    //blockchain::verify_blocks(db);

    cout << "*** Cleaning Up" << endl;
    delete db;
    delete dest;

    return a.exec();
}
*/
