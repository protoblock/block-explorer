#include <QApplication>
#include <QQmlApplicationEngine>

#include <QtQml>

#include <display.h>

/*
Blockchain
Protocol
Mutable
Operator
Command
Action
 */

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    Display dsply;

    engine.rootContext()->setContextProperty("Display", &dsply);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    QObject *item = engine.rootObjects().at(0);

    QObject::connect(item, SIGNAL(nextPressed()),
                     &dsply, SLOT(nextPressedSlot()));
    QObject::connect(item, SIGNAL(prevPressed()),
                     &dsply, SLOT(prevPressedSlot()));



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
