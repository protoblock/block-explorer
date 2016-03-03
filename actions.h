#ifndef ACTIONS_H
#define ACTIONS_H

#include <proto/ProtoData.pb.h>
#include <fc/crypto/sha256.hpp>
#include <fc/crypto/elliptic.hpp>
#include <fc/filesystem.hpp>

#include <blockchain.h>

#include "proto/StateData.pb.h"

using namespace std;

class Actions
{
public:
    Actions();

    static fantasybit::MerkleTree CreateMerkle(fantasybit::Block &block);

    static void Test();
};

#endif // ACTIONS_H
