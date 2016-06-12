#ifndef CREATESTATE_H
#define CREATESTATE_H
#include <string>
#include "proto/StateData.pb.h"
#include <fc/crypto/sha256.hpp>
#include "ldbwriter.h"
#include "utils.h"
#include "unordered_map"
#include "datastores.h"
#include "stdint.h"

namespace fantasybit_bx {

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

    MerkleTree m_posmetatree;
    std::unordered_map<std::string,PosMeta> m_posmetamap;


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
    std::string createState(const fantasybit_bx::BlockMeta &bm );

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
    void loadSubStates();

    void loadPlayerState();

    void loadGameState();

    void loadFantasyNameBalState();

    void loadProjState();

    void loadOrderState();

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
    void createTrDataState();

    /**
     * @brief createTrPlayerDataState write and update latest team/player states
     */
    void createTrPlayerDataState();

    /**
     * @brief createTrGameDataState write and update latest game/schedule/result states
     */
    void createTrGameDataState();

    void processTr(const TrMeta &trmeta, const std::string &trid);

    void processTx(const std::string &txmetaid);

    void processNameTx(std::unordered_map<std::string, TxMeta> &tmap);

    void processRegTx(std::unordered_map<std::string, TxMeta> &tmap,MerkleTree &tree);

    void processGameStart(const std::string &gmid,const GameMeta &gmeta,const std::string &trid);

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
                  std::unordered_map<std::string,  T> &mapt) {

        ldb.read(in,mtree);
        for ( auto nodestr : mtree.leaves()) {
            T nodet;
            ldb.read(nodestr,nodet);
            mapt[nodestr] = nodet;
        }

        return mtree.root();
    }

    template<class T>
    std::string loadMerkleMap(const std::string &in, MerkleTree &mtree,
                  std::vector< std::pair<std::string,  T> > &mapt) {

        ldb.read(in,mtree);
        for ( auto nodestr : mtree.leaves()) {
            T nodet;
            ldb.read(nodestr,nodet);
            mapt.push_back(make_pair(nodestr,nodet));
        }

        return mtree.root();
    }

    template<class T>
    std::string setNewMerkelTree(const std::unordered_map<std::string,  T>  &mapt,
                                 MerkleTree &mtree) {
        for ( auto &leaf : mapt) {
            mtree.add_leaves(leaf.first);
        }
        mtree.set_root(makeMerkleRoot(mtree.leaves()));
        ldb.write(mtree.root(),mtree.SerializeAsString());
        return mtree.root();
    }

    template<class T>
    std::string loadMerkleMap(const std::string &in,std::vector<T> &vect) {

        MerkleTree mtree;
        ldb.read(in,mtree);
        for ( int i = 0; i < mtree.leaves_size(); i++) {
            auto nodestr = mtree.leaves(i);
            T nodet;
            ldb.read(nodestr,nodet);
            vect.push_back(nodet);
        }

        return mtree.root();
    }



    template<class T>
    void dumpMerkleMap(std::unordered_map<std::string,  T> &mapt) {

        for(auto p: mapt) {
            qDebug() << p.first.data();
            qDebug() << p.second.DebugString().data();
        }
    }

    std::string setWriteMerkle(MerkleTree &tree) {
        tree.set_root(makeMerkleRoot(tree.leaves()));
        return ldb.write(tree.root(),tree.SerializeAsString());
    }


    PlayerStore m_playerstore;
    GameStatusStore m_gamestatustore;
    FantasyNameStore m_fantasynamestore;
    ProjStore m_projstore;
    PosStore m_posstore;
    OrderStore m_orderstore;
    MarketStore m_marketstore;
    std::string loadDefaultStates();
    static std::vector<std::string> GENESIS_NFL_TEAMS;

    std::string processTeamGameStart(const std::string &pidroot,const std::string &,const std::string &);
    void createTrState();
    void ProcessInsideStamped(const SignedTransaction &inst, int32_t seqnum);
    void createMarketOrderState();
    void createPosState();
    void fromProj2Results(const TeamProjMeta &teamproj, TeamResultMeta &teamresult, const std::string &statusid, const std::string &trid);
    std::string ProcessResults(const ::google::protobuf::RepeatedPtrField< ::fantasybit_bx::PlayerResult >&in,
                        const std::string &, const std::string &id,
                               const std::string &trid,const std::string &);
    void processGameResult(const GameResult &grslt, const InGameProjMeta &igmeta, const std::string &trid);
    void createTrLeaderboardState();
    void processFirstOrder(const OrderMeta &ordernew, const std::string &newoid);
    std::string processCancelOrder(const std::string &txid, const ExchangeOrder &eo, const std::string &fname, int32_t refnum);
    void createTrMarketState();

    uint64_t getSkillBalance(const std::string &fname) {
        return m_fantasynamestore.getStake(fname);
    }
    std::string processNewOrder(const std::string &txid, const ExchangeOrder &eo, const std::string &fname, int32_t refnum, uint64_t timestamp);
};


}
#endif // CREATESTATE_H
