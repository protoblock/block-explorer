#ifndef BLOCKEXPLORER
#define BLOCKEXPLORER
#include <string>
#include <createmeta.h>
#include <ldbwriter.h>

using namespace std;
namespace fantasybit_bx {


class BlockExplorer {
public:

    BlockExplorer() : cm(ldb) {
        prev = "";
    }

    CreateMeta cm;
    std::string prev;
    int height;
    int start;
    LdbWriter ldb;

    int init(const std::string &startread) {
        BlockMeta bm;

        auto headid = ldb.read(startread);
        ldb.read(headid,bm);

        start = bm.blocknum();
        prev = headid;
        return start;
    }

    int init(int start) {
        string sread = "blocknum_"+ to_string(start);
        return init(sread);
    }

    int init() {
        return init("blockhead");
    }

    void pblock(const fantasybit_bx::Block &b) {
        prev = cm.DoMeta(b,prev);
        ldb.write("blocknum_" + to_string(b.signedhead().head().num()),prev);
    }

    void endit() {
        if ( prev != "")
            ldb.write("blockhead",prev);
    }

    uint64_t SkillBalance(const std::string &fname) {
        return cm.getSkillBalance(fname);
    }
};
}
#endif // BLOCKEXPLORER

