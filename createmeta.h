#ifndef CREATEMETA_H
#define CREATEMETA_H

#include "proto/StateData.pb.h"
#include "proto/ProtoData.pb.h"
#include <fc/crypto/sha256.hpp>
#include <qdebug>
#include <leveldb/db.h>
#include <iostream>
#include "google/protobuf/descriptor.pb.h"
#include "createstate.h"
#include <ldbwriter.h>

namespace fantasybit_bx {

//using namespace std;


class CreateMeta
{
    LdbWriter &writer;
    CreateState bState;
public:
    CreateMeta(LdbWriter &idb) : writer(idb), bState(idb) {
        idb.init();
        bState.init();
    }

    std::string DoMeta(const Block &,const std::string &);
    TxMeta createTxMeta(int32_t bnum, int32_t tnum, const SignedTransaction &st);
    std::string DoMetaTr(int32_t bnum, const SignedTransaction &st);
    std::string DoMetaTx(int32_t bnum, const Block &b);

    uint64_t getSkillBalance(const std::string &fname) {
        return bState.getSkillBalance(fname);
    }

};


}

#endif // CREATEMETA_H
