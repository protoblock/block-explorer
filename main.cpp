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

    cout << "*** init" << endl;

    Int32Comparator *cmp = new Int32Comparator();
    leveldb::Options optionsInt;
    optionsInt.create_if_missing = true;
    optionsInt.comparator = cmp;

    leveldb::Status status;

    leveldb::DB *db;
    status = leveldb::DB::Open(optionsInt, "/Users/satoshi/ryan_work/block/blockchain", &db);
    if (!status.ok()) {
        cout << " error opening block/blockchain" << endl;
        return 0;
    }

    // Print out block
    //displayBlock(db, 200);

    // Print out signed headers
    //displayHeaders(db);

    // Print Diagnostics
    //displayDiagnostics(db);

    blockchain::test(db);

    /*
    string s = "Howdy";
    fc::sha256 digest = fc::sha256::hash(s);

    cout << s << ": sha256 = " << (string) digest << endl;

    string ss = "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";

    fc::sha256 f = (fc::sha256) ss;

    cout << ss << endl;

    cout << (string) f << endl;
    */

    cout << "*** Cleaning Up" << endl;
    delete db;

    return a.exec();
}
