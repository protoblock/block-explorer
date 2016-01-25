// Ryan Ragle created this

#include <QCoreApplication>
#include <iostream>
#include <leveldb/db.h>
#include <leveldb/comparator.h>
#include <ProtoData.pb.h>
#include <utils.h>

#include <fc/crypto/sha256.hpp>
#include <fc/crypto/elliptic.hpp>
#include <fc/filesystem.hpp>

#include <blockchain.h>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    blockchain bc{};

    cout << "*** Block Height: " << bc.GetBlockHeight() << endl;

    //bc.SeekToFirst();
    //cout << "*** Genesis Block: " << bc.GetCurrentBlock().DebugString() << endl;

    //bc.Seek(2);
    //cout << "Block 2: " << bc.GetCurrentBlock().DebugString() << endl;

    //void Next();
    //void Prev();
    //bool Valid();

    //bc.SeekToLast();
    //cout << "Block " << bc.GetBlockHeight() << ": " << bc.GetCurrentBlock().DebugString() << endl;

    // Print First 10 Blocks
    //bc.SeekToFirst();
    //for (int i = 0; i < 10 && bc.Valid(); ++i, bc.Next()) {
    //    fantasybit::Block b = bc.GetCurrentBlock();
    //    cout << "Block " << b.signedhead().head().num() << ": " << b.signedhead().DebugString() << endl;
    //}

    // Print Last 10 Blocks
    bc.SeekToLast();
    for (int i = 10; i > 0 && bc.Valid(); --i, bc.Prev()) {
        fantasybit::Block b = bc.GetCurrentBlock();
        cout << "Block " << b.signedhead().head().num() << ": " << b.signedhead().DebugString() << endl;
    }

    // Note: When executing, the program must finish in order to prevent locks
    return a.exec();
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
