#ifndef CREATESTATE_H
#define CREATESTATE_H
#include <string>
#include "proto/StateData.pb.h"
#include <fc/crypto/sha256.hpp>

namespace fantasybit {

class LdbWriter;

class CreateState
{
    LdbWriter &ldb;
public:
    CreateState(LdbWriter &writer) : ldb(writer) {}

    void init() {}

    std::string getState(const fantasybit::BlockMeta &bm ) {
        pbstate pbs{};


        auto pbstatestr =  pbs.SerializeAsString();
        auto pbstateid = fc::sha256::hash(pbstatestr).str();

        return pbstateid;
    }
};
}
#endif // CREATESTATE_H
