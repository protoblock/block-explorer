#ifndef CREATESTATE_H
#define CREATESTATE_H
#include <string>
#include "proto/StateData.pb.h"
#include <fc/crypto/sha256.hpp>
#include "ldbwriter.h"
#include "utils.h"
#include "unordered_map"
#include "datastores.h"

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
//        optional bytes teamstateid = 70; //teammetaroot
//        optional bytes fantasynamestateid = 80; //FantasyNameState
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
//    }`

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


    MerkleTree m_teamstatetree;
    std::unordered_map<std::string,TeamMeta> m_teamstatemap;
//    //teammetaid -> teammeta
//    //teammetaidroot -> MerkleTree<teammetaid>
//    //playermetaidroot -> MerkleTree<playermetaid>
//    message TeamMeta {
//        optional string chars = 10;
//        optional bytes playermetaidroot = 20;
//        optional bytes prev = 30;
//    }

    MerkleTree m_fantasynamestatetree;
    std::unordered_map<std::string,FantasyNameState> m_fantasynamestatemap;
//    //FantasyNameState - state of Fname
//    //  positions - open positions`
//    //  orders - openorders
//    //  proj - ope projections
//    message FantasyNameState {
//        optional string name = 1;
//        optional bytes fantasynamebalmetaid = 10;
//        optional bytes posmetaroot = 20;
//        optional bytes ordersmetaroot = 30;
//        optional bytes projmetaroot = 40;
//        optional bytes prev = 70;
//        optional bytes next = 80;
//        optional bytes txmetaid = 50;
//    }


public:
    CreateState(LdbWriter &writer) : ldb(writer) {}

    void init() {}

    void dump(const std::string &pbstateid) ;
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

    /**
     * @brief loadSubStates load all state data pointed to by pbstate
     */
    void loadSubStates() {
        loadPlayerState();
        loadGameState();
        loadFantasyNameBalState();
        loadProjState();
    }

    void loadPlayerState();

    void loadGameState();

    void loadFantasyNameBalState();

    void loadProjState();

    /**
     * @brief process blockmeta in order
     *      1. transition data
     *      2. name transactions
     *      3. other transactions
     *      4. transition
     * @param bm
     */
    void process(const BlockMeta &bm);

    /**
     * @brief processTrData process new player and game data from block
     * @param datametaroot
     */
    void processTrData(const std::string &datametaroot);

    /**
     * @brief createTrDataState write new player and game states post data processing
     */
    void createTrDataState() {
        createTrPlayerDataState();
    }

    /**
     * @brief createTrPlayerDataState write and update latest team/player states
     */
    void createTrPlayerDataState();

    void processTr(const TrMeta &trmeta, const std::string &trid) {
        GlobalState gs;
        switch(trmeta.type()) {
        case TrType::WEEKOVER:
            if ( trmeta.week()+1 > 16 ) {
                gs.set_week(0);
                gs.set_season(trmeta.season()+1);
                gs.set_state(GlobalState_State_OFFSEASON);
            }
            gs.set_week(trmeta.week()+1);
            m_globalstatemeta.set_prev(m_pbstate.globalstateid());
            m_globalstatemeta.set_trmetaid(trid);
            m_globalstatemeta.mutable_globalstate()->CopyFrom(gs);
            m_pbstate.set_globalstateid(ldb.write(m_globalstatemeta));

            break;
        case TrType::GAMESTART:

            m_globalstatemeta.set_prev(m_pbstate.globalstateid());
            m_globalstatemeta.set_trmetaid(trid);
            m_globalstatemeta.mutable_globalstate()->CopyFrom(gs);
            m_pbstate.set_globalstateid(ldb.write(m_globalstatemeta));

            break;
        case TrType::SEASONEND:
            gs.set_state(GlobalState_State_OFFSEASON);
            gs.set_season(trmeta.season()+1);
            m_globalstatemeta.set_prev(m_pbstate.globalstateid());
            m_globalstatemeta.set_trmetaid(trid);
            m_globalstatemeta.mutable_globalstate()->CopyFrom(gs);
            m_pbstate.set_globalstateid(ldb.write(m_globalstatemeta));

            break;
        case TrType::TRADESESSIONSTART:
            m_globalstatemeta.set_prev(m_pbstate.globalstateid());
            m_globalstatemeta.set_trmetaid(trid);
            m_pbstate.set_globalstateid(ldb.write(m_globalstatemeta));

            break;
        case TrType::SEASONSTART:
            gs.set_state(GlobalState_State_INSEASON);
            gs.set_season(trmeta.season());
            gs.set_week(trmeta.week());
            m_globalstatemeta.mutable_globalstate()->CopyFrom(gs);
            m_globalstatemeta.set_prev(m_pbstate.globalstateid());
            m_globalstatemeta.set_trmetaid(trid);
            m_pbstate.set_globalstateid(ldb.write(m_globalstatemeta));
            break;
        case TrType::HEARTBEAT:
            if ( !m_globalstatemeta.has_globalstate() ) {
                gs.set_state(GlobalState_State_INSEASON);
                gs.set_season(trmeta.season());
                gs.set_week(trmeta.week());
                m_globalstatemeta.mutable_globalstate()->CopyFrom(gs);
                m_globalstatemeta.set_prev(m_pbstate.globalstateid());
                m_globalstatemeta.set_trmetaid(trid);
                m_pbstate.set_globalstateid(ldb.write(m_globalstatemeta));

            }
        default:
            break;

        }
    }

    void processTx(const std::string &txmetaid);

    void processNameTx(std::unordered_map<std::string, TxMeta> &tmap);

    void processRegTx(std::unordered_map<std::string, TxMeta> &tmap);

    /**
     * @brief createTxState write new state data post tx processing
     */
    void createTxState();

    /**
     * @brief createNameTxState write and update latest fantasyname states
     */
    void createNameTxState();

    void createProjState();

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

    template<class T>
    void dumpMerkleMap(std::unordered_map<std::string,typename T> &mapt) {

        for(auto p: mapt) {
            qDebug() << p.first.data();
            qDebug() << p.second.DebugString().data();
        }
    }

    PlayerStore m_playerstore;
    GameStatusStore m_gamestatustore;
    FantasyNameStore m_fantasynamestore;
    ProjStore m_projstore;
    std::string loadDefaultStates();
    static std::vector<std::string> GENESIS_NFL_TEAMS;

};


}
#endif // CREATESTATE_H
