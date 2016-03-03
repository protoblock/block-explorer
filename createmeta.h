#ifndef CREATEMETA_H
#define CREATEMETA_H

#include "proto/StateData.pb.h"
#include "proto/ProtoData.pb.h"
#include <fc/crypto/sha256.hpp>
#include <qdebug>
#include <leveldb/db.h>
#include <iostream>

namespace fantasybit {

using namespace std;

class LdbWriter {
    leveldb::DB *db;
    leveldb::WriteOptions write_sync{};

public:

    void LdbWriter::init() {
        write_sync.sync = true;

        leveldb::Options opt;
        opt.create_if_missing = true;
        opt.error_if_exists = false;

        leveldb::Status status;

        leveldb::DB *db;
        status = leveldb::DB::Open(opt, "./block/meta", &db);
        if (!status.ok()) {
            cout << "!!! error opening db block/meta" << endl;
            cout << "!!! " << status.ToString() << endl;
            exit(1);
        }

        this->db = db;
    }


    void write(const string &key, const string &val) {
        //qDebug() << key.data() << val.data();
        db->Put(write_sync,key,val);
    }
    void write(const string &val) {
        write(fc::sha256::hash(val).str(),val);
    }

    void write(const google::protobuf::Message &msg) {
        write(msg.SerializeAsString());
    }

    string read(const string &id) {
        string value;
        db->Get(leveldb::ReadOptions(),id,&value);
        return value;
    }

    ~LdbWriter() {
        if ( db ) delete db;
    }
};

class CreateMeta
{
    LdbWriter &writer;
public:
    CreateMeta(LdbWriter &idb) : writer(idb) {}

    string DoMeta(const Block &,const string &);
    TxMeta CreateMeta::createTxMeta(int32_t bnum, int32_t tnum, const SignedTransaction &st);
    string CreateMeta::DoMetaTr(int32_t bnum, const SignedTransaction &st);
    string CreateMeta::DoMetaTx(int32_t bnum, const Block &b);


};


}

#endif // CREATEMETA_H
