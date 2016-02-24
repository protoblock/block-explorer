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

    void init();
    // LevelDB stuff
    fantasybit::Block GetCurrentBlock() const;
    fantasybit::Block GetBlock(int32_t) const;

    void Seek(int32_t n) const;
    void SeekToFirst() const;
    void SeekToLast() const;
    void Next() const;
    void Prev() const;
    bool Valid() const;

    int32_t GetBlockHeight() const;
    bool Verify() const;

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
