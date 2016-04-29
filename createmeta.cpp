#include "createmeta.h"
#include "utils.h"
#include "createstate.h"
using namespace std;

namespace fantasybit_bx {

string CreateMeta::DoMeta(const Block &b, const string &prev) {
    BlockMeta bm{};
    auto bnum= b.signedhead().head().num();
    bm.set_blocknum(bnum);
    bm.set_prev(prev);

    //auto txm0 = createTxMeta(bnum,0,b.signed_transactions(0));
    bm.set_trmetaid(DoMetaTr(bnum,b.signed_transactions(0)));
    if ( b.signed_transactions_size() > 1 ) {
        bm.set_txmetaroot(DoMetaTx(bnum,b));
    }

    bm.set_pbstateid(bState.createState(bm));

    auto bms = bm.SerializeAsString();
    auto bmsid = fc::sha256::hash(bms).str();
    writer.write(bmsid,bms);

    return bmsid;
}

string CreateMeta::DoMetaTx(int32_t bnum, const Block &b) {
    MerkleTree mt{};
    for (int i = 1; i < b.signed_transactions_size(); i++) {
        auto &st = b.signed_transactions(i);
        const auto &txm = createTxMeta(bnum,i,st);
        auto txms = txm.SerializeAsString();
        auto txmid = fc::sha256::hash(txms).str();
        writer.write(txmid,txms);
        mt.add_leaves(txmid);
    }
    mt.set_root(makeMerkleRoot(mt.leaves()));
    writer.write(mt.root(),mt.SerializeAsString());

    return mt.root();
}

string CreateMeta::DoMetaTr(int32_t bnum, const SignedTransaction &st) {
    auto &dt = st.trans().GetExtension(DataTransition::data_trans);
    TrMeta tr{};
    tr.set_blocknum(bnum);
    tr.set_type(dt.type());
    tr.set_season(dt.season());
    tr.set_week(dt.week());



    if ( bnum == 139 )
        qDebug() << " 139 " << dt.DebugString().data();
    auto txm = createTxMeta(bnum,0,st).SerializeAsString();
    auto txmid = fc::sha256::hash(txm).str();
    writer.write(txmid,txm);

    tr.set_txmetaid(txmid);
    if ( dt.gamedata_size() > 0) {
        MerkleTree mt{};
        for ( auto gd : dt.gamedata()) {
            GameMeta gm{};
            gm.mutable_gamedata()->CopyFrom(gd);
            gm.set_txmetaid(txmid);
            auto gmstr = gm.SerializeAsString();
            auto gmsid = fc::sha256::hash(gmstr).str();
            writer.write(gmsid,gmstr);
            mt.add_leaves(gmsid);
        }
        mt.set_root(makeMerkleRoot(mt.leaves()));
        writer.write(mt.root(),mt.SerializeAsString());
        tr.set_gamemetaroot(mt.root());
    }

    if ( dt.data_size() > 0) {
        MerkleTree mt{};
        for ( auto gd : dt.data()) {
            DataMeta gm{};
            gm.mutable_data()->CopyFrom(gd);
            gm.set_txmetaid(txmid);
            auto gmstr = gm.SerializeAsString();
            auto gmsid = fc::sha256::hash(gmstr).str();
            writer.write(gmsid,gmstr);
            mt.add_leaves(gmsid);
            if ( bnum == 139 )
                qDebug() << " DataMeta " << gm.DebugString().data();
        }
        mt.set_root(makeMerkleRoot(mt.leaves()));
        writer.write(mt.root(),mt.SerializeAsString());
        tr.set_datametaroot(mt.root());
    }

    auto str = tr.SerializeAsString();
    auto trid = fc::sha256::hash(str).str();
    writer.write(trid,str);
    return trid;
}

TxMeta CreateMeta::createTxMeta(int32_t bnum, int32_t tnum,
                                const SignedTransaction &st) {
    TxMeta txm{};
    txm.set_blocknum(bnum);
    txm.set_txnum(tnum);
    txm.mutable_tx()->CopyFrom(st.trans());
    txm.set_txtype(st.trans().type());
    txm.set_fantasy_name(st.fantasy_name());
    txm.set_id(st.id());

    return txm;
}

/*
MerkleTree createTxMetaTree(const Block &block) {
    MerkleTree mt{};
    if ( block.signed_transactions_size() < 2)
        return mt;

    for ( int i = 1; i<block.signed_transactions_size(); i++) {

    }
}
*/

}

