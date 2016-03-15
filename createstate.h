#ifndef CREATESTATE_H
#define CREATESTATE_H
#include <string>
#include "proto/StateData.pb.h"

namespace fantasybit {

class LdbWriter;

class CreateState
{
    LdbWriter &ldb;
public:
    CreateState(LdbWriter &writer) : ldb(writer) {}

    void init() {}

    std::string getState(const fantasybit::BlockMeta &bm ) {
        return "";
    }
};
}
#endif // CREATESTATE_H
