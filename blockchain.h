#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <iostream>
#include <leveldb/db.h>
#include <leveldb/comparator.h>
#include <ProtoData.pb.h>
#include <utils.h>

#include <fc/crypto/sha256.hpp>
#include <fc/crypto/elliptic.hpp>
#include <fc/filesystem.hpp>

using namespace std;

class Blockchain
{
protected:
    int32_t block_height;
    leveldb::DB *db;
    leveldb::Iterator* it;

public:
    Blockchain();
    ~Blockchain();

    // LevelDB stuff
    fantasybit::Block GetCurrentBlock();
    void Seek(int32_t n);
    void SeekToFirst();
    void SeekToLast();
    void Next();
    void Prev();
    bool Valid();

    int32_t GetBlockHeight();
    bool Verify();

    static void Test();

    /*
    static fc::sha256 create_merkle(fantasybit::Block block);
    static void test(leveldb::DB *db);
    static void new_blockchain(leveldb::DB *src, leveldb::DB *dest);
    static bool verify_block(fantasybit::Block block);
    static void verify_blocks(leveldb::DB *db);
    */
};

#endif // BLOCKCHAIN_H
