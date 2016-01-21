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

class blockchain
{
public:
    blockchain();
    static fc::sha256 create_merkle(fantasybit::Block block);
    static void test(leveldb::DB *db);
    static void new_blockchain(leveldb::DB *src, leveldb::DB *dest);

};

#endif // BLOCKCHAIN_H
