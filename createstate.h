#ifndef CREATESTATE_H
#define CREATESTATE_H
#include <string>
#include "proto/StateData.pb.h"
#include <fc/crypto/sha256.hpp>
#include "ldbwriter.h"
#include "utils.h"
#include "unordered_map"

namespace fantasybit {

/**
 * @brief The CreateState class is used to create state during creation of BlockMeta,
 * uses previous state wither from database or memory
 */
class CreateState
{
    LdbWriter &ldb;

    std::string m_pbstateid ;
    pbstate m_pbstate;
//    message pbstate {
//        optional bytes schedulestateid = 10; //WeekGameStatusMetaRoot
//        optional bytes leaderboardstateid = 20; //LeaderBoardMetaId
//        optional bytes projstateid = 30; //ProjMetaroot
//        optional bytes orderstateid = 40; //OrderMetaRoot
//        optional bytes marketstateid = 50; //PlayerMarketStateRoot
//        optional bytes globalstateid = 60; //GlobalStateMetaId
//    }

    MerkleTree m_weekgamestatusmetatree;
    std::unordered_map<std::string,WeekGameStatusMeta> m_weekgamestatusmetamap;
//    //gameresultmetaroot -> MerkleTree<gameresultmetaid>
//    //gameresultmetaid -> GameResultsMeta
//    //ingameprojmetaroot -> MerkleTree<InGameProjMetaroot>
//    //gameprojmetaid -> GameProjMeta
//    message WeekGameStatusMeta {
//        optional uint32 week = 10;
//        optional bytes gameresultmetaroot = 20;
//        optional bytes ingameprojmetaroot = 30;
//        optional bytes opengamestatusroot = 40;
//        optional bytes prev = 50;
//        optional bytes next = 60;
//    }

    LeaderBoardMeta m_leaderboardmeta;
//    //LeaderBoardMeta - State of all players and balances
//    //LeaderBoardMetaId -> LeaderBoardMeta
//    //fnbalmetaroot -> MerkleTree<fnbalmetaid>
//    message LeaderBoardMeta {
//        //optional GlobalState global = 10;
//        optional uint32 week = 10;
//        optional bytes fnbalmetaroot = 20;
//        optional bytes prev = 30;
//        optional bytes next = 40;
//    }

    MerkleTree m_projmetatree;
    std::unordered_map<std::string,ProjMeta> m_projmetamap;
//    //projmetaid -> ProjMeta
//    message ProjMeta {
//        optional FantasyBitProj fantaybitproj = 10;
//        optional bytes txmetaid = 20;
//        optional bytes prev = 30;
//    }

    MerkleTree m_ordermetatree;
    std::unordered_map<std::string,OrderMeta> m_ordermetamap;
//    message OrderMeta {
//        optional string fname = 10;
//        optional string playerid = 20;
//        optional Order  order = 30;
//        optional bytes txmetaid = 40;
//        optional bytes orderfillmetaroot = 50;
//        optional bytes prev = 60;
//    }

    MerkleTree m_playermarketstatetree;
    std::unordered_map<std::string,PlayerMarketState> m_playermarketstatemap;
//    message PlayerMarketState {
//        optional bytes limitbookmetaid = 10;
//        optional string playerid = 20;
//        optional bytes marketticmetaid = 30;
//        optional bytes prev = 40;
//        optional bytes next = 50;
//    }

    GlobalStateMeta m_globalstatemeta;
//    message GlobalStateMeta {
//        optional GlobalState globalstate = 10;
//        optional bytes trmetaid = 20;
//        optional bytes prev = 30;
//        optional bytes next = 40;
//    }

public:
    CreateState(LdbWriter &writer) : ldb(writer) {}

    void init() {}

    /**
     * @brief createState create pbstate and add to BlockMeta
     * @param bm
     * @return
     */
    std::string createState(const fantasybit::BlockMeta &bm );

    /**
     * @brief loadStateId - load state into memory from blockmetaid
     * @param blockmetaid
     * @return
     */
    std::string loadStateId(const std::string &blockmetaid);

    /**
     * @brief getStateId - return pbstatemetaod of blockmetaid
     * @param blockmetaid
     * @return
     */
    std::string getStateId(const std::string &blockmetaid);

    void process(const BlockMeta &bm) {
        processTx(bm.txmetaroot());
    }

    void processTx(const std::string &txmetaid) {

    }


    template<class T>
    std::string loadMerkleMap(const std::string &in, MerkleTree &mtree,
                  std::unordered_map<std::string,typename T> &mapt) {

        ldb.read(in,mtree);
        for ( auto nodestr : mtree.leaves()) {
            typename T nodet;
            ldb.read(nodestr,nodet);
            mapt[nodestr] = nodet;
        }

        return mtree.root();
    }
};

}
#endif // CREATESTATE_H
