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

    void processRegTx(std::unordered_map<std::string, TxMeta> &tmap);

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

    std::map<std::string,int> bracedoc;

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
    std::string ProcessResults(decltype(GameResult::default_instance().home_result()) &in,
                        const std::string &, const std::string &id,
                               const std::string &trid,const std::string &);
    void processGameResult(const GameResult &grslt, const InGameProjMeta &igmeta, const std::string &trid);
    void createTrLeaderboardState();
    std::string processNewOrder(const std::string &txid, const ExchangeOrder &eo, const std::string &fname, int32_t refnum);
    void processFirstOrder(const OrderMeta &ordernew, const std::string &newoid);
    std::string processCancelOrder(const std::string &txid, const ExchangeOrder &eo, const std::string &fname, int32_t refnum);
};


}
#endif // CREATESTATE_H



/*

Starting D:\work\block-explorer\release\blockexplorer.exe...
The program has unexpectedly finished.
D:\work\block-explorer\release\blockexplorer.exe crashed

Starting D:\work\block-explorer\release\blockexplorer.exe...
 self trade
 refnum: 375
fname: "Satoshi Naka-Roto"
playerid: "1125"
fillprice: 8
fillsize: 1
buyside: true
 refnum: 2371
fname: "Satoshi Naka-Roto"
playerid: "1125"
buyside: false
size: 0
price: 2
txmetaid: "c27aab0d0b1196b6b0fb9dcf1dc3af0cc2efc1b87b1a98e18ac14b396180d767"

 self trade
 refnum: 812
fname: "Satoshi Naka-Roto"
playerid: "1566"
fillprice: 2
fillsize: 1
buyside: false
 refnum: 2712
fname: "Satoshi Naka-Roto"
playerid: "1566"
buyside: true
size: 0
price: 2
txmetaid: "0954adaa705863c05b283268a79e2b952a4d0e45596147d76cae21afa727709f"

 self trade
 refnum: 625
fname: "Satoshi Naka-Roto"
playerid: "1636"
fillprice: 3
fillsize: 1
buyside: true
 refnum: 2887
fname: "Satoshi Naka-Roto"
playerid: "1636"
buyside: false
size: 0
price: 3
txmetaid: "44808ed622f0615da98d0c89ce44dd0c0198300f4ea833101755f1c406424d8f"

 self trade
 refnum: 871
fname: "Satoshi Naka-Roto"
playerid: "1126"
fillprice: 3
fillsize: 1
buyside: true
 refnum: 2837
fname: "Satoshi Naka-Roto"
playerid: "1126"
buyside: false
size: 0
price: 2
txmetaid: "5d4e628fea39dadec276d3207c16f7cc2d6c0f34a20ccdc084d6ebac9d47370e"

 self trade
 refnum: 695
fname: "Satoshi Naka-Roto"
playerid: "1357"
fillprice: 5
fillsize: 1
buyside: true
 refnum: 2633
fname: "Satoshi Naka-Roto"
playerid: "1357"
buyside: false
size: 0
price: 5
txmetaid: "1d50586d336602c02fc6045ea58def6525e20588722c575f615af980127841bc"

 self trade
 refnum: 3036
fname: "11111111"
playerid: "1078"
fillprice: 10
fillsize: 1
buyside: false
 refnum: 3408
fname: "11111111"
playerid: "1078"
buyside: true
size: 0
price: 10
txmetaid: "fbb402f17fbcafdff5cd3b9b4f34a554dd7f5345ceca08c80cfa180ff0e7862d"

 self trade
 refnum: 1900
fname: "JayBNY"
playerid: "1126"
fillprice: 3
fillsize: 1
buyside: true
 refnum: 3452
fname: "JayBNY"
playerid: "1126"
buyside: false
size: 0
price: 3
txmetaid: "386c2e4b2df8cd85994dac7572432d468eea851acbc195085b6d469f8f5437cb"

 self trade
 refnum: 1399
fname: "JayBNY"
playerid: "1125"
fillprice: 8
fillsize: 1
buyside: true
 refnum: 3455
fname: "JayBNY"
playerid: "1125"
buyside: false
size: 0
price: 8
txmetaid: "747a5575de6c36f71fb71154204e29f83a3894c447b289159be811efc0408598"

 self trade
 refnum: 1698
fname: "JayBNY"
playerid: "1294"
fillprice: 6
fillsize: 1
buyside: true
 refnum: 3479
fname: "JayBNY"
playerid: "1294"
buyside: false
size: 3
price: 2
txmetaid: "d3d8d4f33bc314cddb3ed440413381b519bd2d564f27f8b1214adaeff7dd340f"

 self trade
 refnum: 1847
fname: "JayBNY"
playerid: "1298"
fillprice: 2
fillsize: 4
buyside: false
 refnum: 3481
fname: "JayBNY"
playerid: "1298"
buyside: true
size: 0
price: 2
txmetaid: "01816b832df007f6fb4bb79d2dcc89e24c1fc0e358e0be131a0789c020c769e9"

 self trade
 refnum: 3538
fname: "JayBNY"
playerid: "1181"
fillprice: 13
fillsize: 1
buyside: false
 refnum: 3539
fname: "JayBNY"
playerid: "1181"
buyside: true
size: 0
price: 13
txmetaid: "fc2954b74375aa2145fa4934a97fd8a210130d8ca104028fa409363f6fb0b606"

 self trade
 refnum: 3331
fname: "11111111"
playerid: "1427"
fillprice: 13
fillsize: 1
buyside: false
 refnum: 3555
fname: "11111111"
playerid: "1427"
buyside: true
size: 0
price: 13
txmetaid: "d5fca7ad8374f73683b8b64cc2dd358d4d47c5ee507e3a512aa7a7e1772f501a"

 self trade
 refnum: 468
fname: "Satoshi Naka-Roto"
playerid: "1518"
fillprice: 2
fillsize: 1
buyside: false
 refnum: 3577
fname: "Satoshi Naka-Roto"
playerid: "1518"
buyside: true
size: 0
price: 2
txmetaid: "c9440753cb9f341307201122af8b9e65a505e568a63386daba3327b5ea286dfb"

 self trade
 refnum: 3363
fname: "11111111"
playerid: "1509"
fillprice: 7
fillsize: 1
buyside: false
 refnum: 3599
fname: "11111111"
playerid: "1509"
buyside: true
size: 0
price: 7
txmetaid: "0aa120fd0321aac4ea5c07ceeec3a35254bf85f58037c969eb4bc2287cecd243"

 self trade
 refnum: 3227
fname: "11111111"
playerid: "1502"
fillprice: 5
fillsize: 1
buyside: false
 refnum: 3600
fname: "11111111"
playerid: "1502"
buyside: true
size: 0
price: 5
txmetaid: "db7fa68dcb27f5ec3b8a58b7cfeda76138f09d6cfc3350c5d375f499bbc84f5c"

gameid: "201501503"
home_result {
  playerid: "1125"
  result: 23.05
  stats {
    ostats {
      passyds: 281
      passtd: 2
      rushyds: 30
      pint: 2
    }
  }
}
home_result {
  playerid: "1714"
  result: 1.7
  stats {
    ostats {
      rushyds: 5
      recyds: 2
      rec: 1
    }
  }
}
home_result {
  playerid: "1625"
  result: 2.5
  stats {
    ostats {
      rushyds: -2
      recyds: 17
      rec: 1
    }
  }
}
home_result {
  playerid: "1805"
  result: 0
  stats {
    ostats {
    }
  }
}
home_result {
  playerid: "1057"
  result: 2
  stats {
    kstats {
      pa: 2
    }
  }
}
home_result {
  playerid: "1053"
  result: 26.8
  stats {
    ostats {
      recyds: 128
      rectd: 1
      rec: 8
    }
  }
}
home_result {
  playerid: "1658"
  result: 6.6
  stats {
    ostats {
      rushyds: 35
      recyds: 11
      rec: 2
    }
  }
}
home_result {
  playerid: "1058"
  result: 8.1
  stats {
    ostats {
      recyds: 31
      rec: 5
    }
  }
}
home_result {
  playerid: "1055"
  result: 13.6
  stats {
    ostats {
      recyds: 26
      rectd: 1
      rec: 5
    }
  }
}
home_result {
  playerid: "1061"
  result: 1.8
  stats {
    ostats {
      rushyds: 18
    }
  }
}
home_result {
  playerid: "1569"
  result: 1.6
  stats {
    ostats {
      recyds: 6
      rec: 1
    }
  }
}
home_result {
  playerid: "1062"
  result: 9
  stats {
    ostats {
      recyds: 60
      rec: 3
    }
  }
}
home_result {
  playerid: "3"
  result: 3
  stats {
    dstats {
      deftd: 0
      sfty: 0
      turnovers: 0
      sacks: 3
      ptsa: 34
    }
  }
}
away_result {
  playerid: "1328"
  result: -0.2
  stats {
    ostats {
      rushyds: -2
    }
  }
}
away_result {
  playerid: "1330"
  result: 7.5
  stats {
    ostats {
      rushyds: 8
      recyds: 27
      rec: 4
    }
  }
}
away_result {
  playerid: "1333"
  result: 14.25
  stats {
    ostats {
      passyds: 171
      passtd: 1
      rushyds: 17
    }
  }
}
away_result {
  playerid: "1346"
  result: 18
  stats {
    ostats {
      recyds: 50
      rectd: 1
      rec: 7
    }
  }
}
away_result {
  playerid: "1350"
  result: 16.5
  stats {
    ostats {
      rushyds: 76
      rushtd: 1
      recyds: 9
      rec: 2
    }
  }
}
away_result {
  playerid: "1339"
  result: 3.2
  stats {
    ostats {
      recyds: 12
      rec: 2
    }
  }
}
away_result {
  playerid: "1345"
  result: 1.4
  stats {
    ostats {
      rushyds: 14
    }
  }
}
away_result {
  playerid: "1336"
  result: 13.3
  stats {
    ostats {
      recyds: 73
      rec: 6
    }
  }
}
away_result {
  playerid: "1351"
  result: 12.9
  stats {
    kstats {
      pa: 4
      fg: 39
      fg: 50
    }
  }
}
away_result {
  playerid: "16"
  result: 20
  stats {
    dstats {
      deftd: 2
      sfty: 0
      turnovers: 3
      sacks: 2
      ptsa: 14
    }
  }
}

 self trade
 refnum: 3606
fname: "JayBNY"
playerid: "1230"
fillprice: 18
fillsize: 1
buyside: false
 refnum: 3616
fname: "JayBNY"
playerid: "1230"
buyside: true
size: 0
price: 18
txmetaid: "703d3c2ace095e51061fd7b87602e7719362f70f28a42f666edba95b48a39f49"

 self trade
 refnum: 4013
fname: "JayBNY"
playerid: "1149"
fillprice: 11
fillsize: 1
buyside: true
 refnum: 5315
fname: "JayBNY"
playerid: "1149"
buyside: false
size: 0
price: 11
txmetaid: "32d625886098603fa21952921e62332963b38bbe361157531812569f5ac00a27"

 self trade
 refnum: 3913
fname: "JayBNY"
playerid: "1351"
fillprice: 9
fillsize: 1
buyside: true
 refnum: 5317
fname: "JayBNY"
playerid: "1351"
buyside: false
size: 0
price: 9
txmetaid: "fc41e2cfec2a84e0c39166b63aa17df37f41a34b3253a2686c371e9fe4785095"

 self trade
 refnum: 5466
fname: "clarity"
playerid: "1266"
fillprice: 10
fillsize: 1
buyside: true
 refnum: 5468
fname: "clarity"
playerid: "1266"
buyside: false
size: 0
price: 10
txmetaid: "3225104e45848c0b7207216e27a93f0163e33a44b67ab21f65dcaa6a56785cd5"

 self trade
 refnum: 5555
fname: "11111111"
playerid: "1681"
fillprice: 15
fillsize: 1
buyside: false
 refnum: 5605
fname: "11111111"
playerid: "1681"
buyside: true
size: 0
price: 15
txmetaid: "0b142cbe48ec9c0d561dfb70a8e186a53162030cdb5ed9805a9924804f64bc40"

 self trade
 refnum: 5688
fname: "Jets Jets Jets"
playerid: "1285"
fillprice: 7
fillsize: 1
buyside: true
 refnum: 5888
fname: "Jets Jets Jets"
playerid: "1285"
buyside: false
size: 0
price: 7
txmetaid: "d89bd18c4940323b8422245ae5e9a0fff32dc28f3c3853c3743e4c08e9004e5d"

 self trade
 refnum: 5620
fname: "Jets Jets Jets"
playerid: "1592"
fillprice: 4
fillsize: 1
buyside: false
 refnum: 5891
fname: "Jets Jets Jets"
playerid: "1592"
buyside: true
size: 0
price: 5
txmetaid: "014bb65a85773466be2949175739b2bfdd18de7987b81b40dc7e6b8a4a71a7b5"

 self trade
 refnum: 5624
fname: "Jets Jets Jets"
playerid: "1413"
fillprice: 3
fillsize: 1
buyside: false
 refnum: 5895
fname: "Jets Jets Jets"
playerid: "1413"
buyside: true
size: 0
price: 4
txmetaid: "7ca4c12e1f6d9df93a747ebd0e621c312934089e8dc39c81f44c682c46f0dcb2"

 self trade
 refnum: 5628
fname: "Jets Jets Jets"
playerid: "1359"
fillprice: 4
fillsize: 1
buyside: false
 refnum: 5899
fname: "Jets Jets Jets"
playerid: "1359"
buyside: true
size: 0
price: 5
txmetaid: "c668e2b81775d9939ab18354969cad89959cd585821d347f5f082cbfc5459a27"

 self trade
 refnum: 5638
fname: "Jets Jets Jets"
playerid: "1375"
fillprice: 3
fillsize: 1
buyside: false
 refnum: 5911
fname: "Jets Jets Jets"
playerid: "1375"
buyside: true
size: 0
price: 4
txmetaid: "d160387d61811c2901d8d3e207daecd8136aa9b84aa132dade670869bcd6dddc"

 self trade
 refnum: 5643
fname: "Jets Jets Jets"
playerid: "1400"
fillprice: 13
fillsize: 1
buyside: false
 refnum: 5915
fname: "Jets Jets Jets"
playerid: "1400"
buyside: true
size: 0
price: 16
txmetaid: "2f472ba86914ecdd0f2f2a91fae0fbe1a4cc1a61a67853a866b7b17365106a8a"

 self trade
 refnum: 5626
fname: "Jets Jets Jets"
playerid: "1393"
fillprice: 3
fillsize: 1
buyside: false
 refnum: 5897
fname: "Jets Jets Jets"
playerid: "1393"
buyside: true
size: 0
price: 4
txmetaid: "c8df1d7432e4a1ffd26de0da846d8fb6bc600d30783e67229615994a0bffb49c"

 self trade
 refnum: 5636
fname: "Jets Jets Jets"
playerid: "1326"
fillprice: 15
fillsize: 1
buyside: false
 refnum: 5909
fname: "Jets Jets Jets"
playerid: "1326"
buyside: true
size: 0
price: 18
txmetaid: "2a2755dff7d2cf46a84f34e70faa56e3a332d8a424321641fad4753fdafc6679"

 self trade
 refnum: 5646
fname: "Jets Jets Jets"
playerid: "1019"
fillprice: 8
fillsize: 1
buyside: false
 refnum: 5919
fname: "Jets Jets Jets"
playerid: "1019"
buyside: true
size: 0
price: 10
txmetaid: "ca0a4bb8e90dfe6f8dc97e6a90386dfdd94189040179461a931c8a9512fbf61b"

 self trade
 refnum: 5656
fname: "Jets Jets Jets"
playerid: "1189"
fillprice: 8
fillsize: 1
buyside: false
 refnum: 5933
fname: "Jets Jets Jets"
playerid: "1189"
buyside: true
size: 0
price: 10
txmetaid: "603cdcc25d236c863274d201707cea8e4a6210b46cfee231fb2c5c4abaea41fe"

 self trade
 refnum: 5664
fname: "Jets Jets Jets"
playerid: "1818"
fillprice: 2
fillsize: 1
buyside: false
 refnum: 5939
fname: "Jets Jets Jets"
playerid: "1818"
buyside: true
size: 0
price: 3
txmetaid: "d50e0b855f1719e2fe3dc1e99d95ece5c31f240c32cd3ed23e4a3e87becb907d"

 self trade
 refnum: 5667
fname: "Jets Jets Jets"
playerid: "1025"
fillprice: 2
fillsize: 1
buyside: false
 refnum: 5943
fname: "Jets Jets Jets"
playerid: "1025"
buyside: true
size: 0
price: 3
txmetaid: "82a8f0945352a749f8f86995f0febd701d9e93a57a0e81af0614393ec7774785"

 self trade
 refnum: 5648
fname: "Jets Jets Jets"
playerid: "1099"
fillprice: 4
fillsize: 1
buyside: false
 refnum: 5921
fname: "Jets Jets Jets"
playerid: "1099"
buyside: true
size: 0
price: 5
txmetaid: "17e241b88cfdfadf25b50703ed03fa6da02d5bd4f4816f2e434e6a3ab8fa5032"

 self trade
 refnum: 5658
fname: "Jets Jets Jets"
playerid: "1394"
fillprice: 5
fillsize: 1
buyside: false
 refnum: 5935
fname: "Jets Jets Jets"
playerid: "1394"
buyside: true
size: 0
price: 7
txmetaid: "2d032eb90c03a21e100cd9ffe16e7bbda97f116d74adf23f36b9a750319dd51e"

 self trade
 refnum: 5652
fname: "Jets Jets Jets"
playerid: "1658"
fillprice: 4
fillsize: 1
buyside: false
 refnum: 5929
fname: "Jets Jets Jets"
playerid: "1658"
buyside: true
size: 0
price: 6
txmetaid: "5c6ae56b00f84e9dc603da2114dae77a152bfa829df2a1c0929be8b1a1fbd80f"

 self trade
 refnum: 5650
fname: "Jets Jets Jets"
playerid: "1763"
fillprice: 4
fillsize: 1
buyside: false
 refnum: 5927
fname: "Jets Jets Jets"
playerid: "1763"
buyside: true
size: 0
price: 5
txmetaid: "ebb0dd1e7e7c7af361516cdd3b558684ed62e58435bc86365d7f477a3facacac"

 self trade
 refnum: 5654
fname: "Jets Jets Jets"
playerid: "1613"
fillprice: 8
fillsize: 1
buyside: false
 refnum: 5931
fname: "Jets Jets Jets"
playerid: "1613"
buyside: true
size: 0
price: 11
txmetaid: "2f6a53edcf5930e83dcc88a3bec697d6fc557ba7de1f01fb529e1fb3cb5e2f01"

 self trade
 refnum: 5714
fname: "Jets Jets Jets"
playerid: "1572"
fillprice: 3
fillsize: 1
buyside: false
 refnum: 5959
fname: "Jets Jets Jets"
playerid: "1572"
buyside: true
size: 0
price: 4
txmetaid: "dce49efba9139655d73f762fc1551b3e1cca31c51109389cac4049e21e266c70"

 self trade
 refnum: 5711
fname: "Jets Jets Jets"
playerid: "1083"
fillprice: 5
fillsize: 1
buyside: false
 refnum: 5957
fname: "Jets Jets Jets"
playerid: "1083"
buyside: true
size: 0
price: 7
txmetaid: "fb479c136795157899ad8f1a3e141415a7d7697f939212d224f1655ed43561de"

 self trade
 refnum: 5695
fname: "Jets Jets Jets"
playerid: "1187"
fillprice: 17
fillsize: 1
buyside: false
 refnum: 5947
fname: "Jets Jets Jets"
playerid: "1187"
buyside: true
size: 0
price: 20
txmetaid: "c759d4bd011cf76998a7a9247bfc0ef930cdbdde21fac9925ee9307b02eb4b42"

 self trade
 refnum: 5705
fname: "Jets Jets Jets"
playerid: "1282"
fillprice: 16
fillsize: 1
buyside: false
 refnum: 5951
fname: "Jets Jets Jets"
playerid: "1282"
buyside: true
size: 0
price: 19
txmetaid: "6bc1ea5b2e1e059e9e1a0b37d32c5d768c32f1bef88cfe9cdf17dcd2ce24da56"

 self trade
 refnum: 5670
fname: "Jets Jets Jets"
playerid: "1633"
fillprice: 3
fillsize: 1
buyside: false
 refnum: 5945
fname: "Jets Jets Jets"
playerid: "1633"
buyside: true
size: 0
price: 4
txmetaid: "d0099e5e359e8cab759255ff15578774691062404e224300b152b59ccecaf1ef"

 self trade
 refnum: 5719
fname: "Jets Jets Jets"
playerid: "1139"
fillprice: 5
fillsize: 1
buyside: false
 refnum: 5967
fname: "Jets Jets Jets"
playerid: "1139"
buyside: true
size: 0
price: 7
txmetaid: "bfa256f2754654b87743eb15dfd3b01058d8fe04c24ecae3e96434588a2bee67"

 self trade
 refnum: 5717
fname: "Jets Jets Jets"
playerid: "1662"
fillprice: 3
fillsize: 1
buyside: false
 refnum: 5963
fname: "Jets Jets Jets"
playerid: "1662"
buyside: true
size: 0
price: 4
txmetaid: "9b62bdb55780f55cf39364f9a291ec9b84e715dd8a0ffac0da865764beef3f6c"

 self trade
 refnum: 5736
fname: "Jets Jets Jets"
playerid: "1086"
fillprice: 5
fillsize: 1
buyside: false
 refnum: 5987
fname: "Jets Jets Jets"
playerid: "1086"
buyside: true
size: 0
price: 7
txmetaid: "d749eb427ca69808d4b1e72a132d5b2e7cc4915d647332b2bded9b786c9ada3c"

 self trade
 refnum: 5739
fname: "Jets Jets Jets"
playerid: "1615"
fillprice: 3
fillsize: 1
buyside: false
 refnum: 5991
fname: "Jets Jets Jets"
playerid: "1615"
buyside: true
size: 0
price: 4
txmetaid: "1d6b2dcafdbaf1cda41fe22831d9f1ae442324c7e60bb9de6435a5c1281b932f"

 self trade
 refnum: 5731
fname: "Jets Jets Jets"
playerid: "1293"
fillprice: 4
fillsize: 1
buyside: false
 refnum: 5983
fname: "Jets Jets Jets"
playerid: "1293"
buyside: true
size: 0
price: 5
txmetaid: "bbbfa5aacdd850652c7859263c81747675e3258e90909f3d96e3365964ef11e9"

 self trade
 refnum: 5725
fname: "Jets Jets Jets"
playerid: "1177"
fillprice: 2
fillsize: 1
buyside: false
 refnum: 5973
fname: "Jets Jets Jets"
playerid: "1177"
buyside: true
size: 0
price: 2
txmetaid: "507f67f9f075d65207126c371fe6e653af525b47e45fe7a9db1a7b1787f1405a"

 self trade
 refnum: 5733
fname: "Jets Jets Jets"
playerid: "1428"
fillprice: 4
fillsize: 1
buyside: false
 refnum: 5985
fname: "Jets Jets Jets"
playerid: "1428"
buyside: true
size: 0
price: 5
txmetaid: "9be511d9afe9800de7e541ff7a75dea714ab3ce2772c20502d8f7d463ab898d7"

 self trade
 refnum: 5761
fname: "Jets Jets Jets"
playerid: "1062"
fillprice: 7
fillsize: 1
buyside: false
 refnum: 6009
fname: "Jets Jets Jets"
playerid: "1062"
buyside: true
size: 0
price: 9
txmetaid: "95d9bdc26bb52691762adb1ce318fecbc5f53f116e95f43f212aa6815c9b9b0d"

 self trade
 refnum: 5745
fname: "Jets Jets Jets"
playerid: "1022"
fillprice: 3
fillsize: 1
buyside: false
 refnum: 5997
fname: "Jets Jets Jets"
playerid: "1022"
buyside: true
size: 0
price: 4
txmetaid: "4d79f9b5cf8f77053958bdc360f60b16698e896566010c3ec0caeeac7e0a4a13"

 self trade
 refnum: 5767
fname: "Jets Jets Jets"
playerid: "1157"
fillprice: 12
fillsize: 1
buyside: false
 refnum: 6015
fname: "Jets Jets Jets"
playerid: "1157"
buyside: true
size: 0
price: 15
txmetaid: "27c109e7e3cd1d8b00e110fda04d75be50273ade5a445dfe2f85c88ce91ae1e0"

 self trade
 refnum: 5750
fname: "Jets Jets Jets"
playerid: "1319"
fillprice: 3
fillsize: 1
buyside: false
 refnum: 6001
fname: "Jets Jets Jets"
playerid: "1319"
buyside: true
size: 0
price: 4
txmetaid: "518b04b2a3586893e2532919ec31e026f4dcbab0868b5d9a620b2398cbe7255c"

 self trade
 refnum: 5765
fname: "Jets Jets Jets"
playerid: "1135"
fillprice: 9
fillsize: 1
buyside: false
 refnum: 6013
fname: "Jets Jets Jets"
playerid: "1135"
buyside: true
size: 0
price: 12
txmetaid: "56a8a36b5ef44cfb588c91ac842b61cd676c8a7f7d45825eefac5828b2bdb19e"

 self trade
 refnum: 5759
fname: "Jets Jets Jets"
playerid: "1431"
fillprice: 14
fillsize: 1
buyside: false
 refnum: 6007
fname: "Jets Jets Jets"
playerid: "1431"
buyside: true
size: 0
price: 17
txmetaid: "884b4ed074aa6cfa9a3ae569744d444316a6fac325e129611a6c7e8f674ff00b"

 self trade
 refnum: 5743
fname: "Jets Jets Jets"
playerid: "1253"
fillprice: 3
fillsize: 1
buyside: false
 refnum: 5995
fname: "Jets Jets Jets"
playerid: "1253"
buyside: true
size: 0
price: 4
txmetaid: "046db93b4095e98745279e4bbaac2098df7808fb924d4baae6d6138b19eab919"

 self trade
 refnum: 5753
fname: "Jets Jets Jets"
playerid: "1625"
fillprice: 7
fillsize: 1
buyside: false
 refnum: 6003
fname: "Jets Jets Jets"
playerid: "1625"
buyside: true
size: 0
price: 9
txmetaid: "df9ddbbbf1c9e248919235e0c969e47cc6f3d79df3b038c5b860db01b567ec86"

 self trade
 refnum: 5757
fname: "Jets Jets Jets"
playerid: "1072"
fillprice: 6
fillsize: 1
buyside: false
 refnum: 6005
fname: "Jets Jets Jets"
playerid: "1072"
buyside: true
size: 0
price: 8
txmetaid: "fe575dfdae2a4f0e98d844ec5ab5e63c55bd70a1699a5fb995748e7b629cceaf"

 self trade
 refnum: 5748
fname: "Jets Jets Jets"
playerid: "1595"
fillprice: 7
fillsize: 1
buyside: false
 refnum: 5999
fname: "Jets Jets Jets"
playerid: "1595"
buyside: true
size: 0
price: 9
txmetaid: "dbfc2edb683a5cf9b912595fe4ce82aa59d379f7ebd98e0ab8e19486807648f8"

 self trade
 refnum: 5741
fname: "Jets Jets Jets"
playerid: "1454"
fillprice: 3
fillsize: 1
buyside: false
 refnum: 5993
fname: "Jets Jets Jets"
playerid: "1454"
buyside: true
size: 0
price: 4
txmetaid: "21c516f728ec170e8be8384137e55f9907971e379a89d09a3b7a6cc69ccbd2ef"

 self trade
 refnum: 5775
fname: "Jets Jets Jets"
playerid: "1277"
fillprice: 3
fillsize: 1
buyside: false
 refnum: 6023
fname: "Jets Jets Jets"
playerid: "1277"
buyside: true
size: 0
price: 4
txmetaid: "59658c593e4f8224549bda6b28fd8a0347787491a70c711e69f7c7c83972e5dc"

 self trade
 refnum: 5777
fname: "Jets Jets Jets"
playerid: "1647"
fillprice: 14
fillsize: 1
buyside: false
 refnum: 6025
fname: "Jets Jets Jets"
playerid: "1647"
buyside: true
size: 0
price: 17
txmetaid: "a61cd02ed0be8ea311e532cde2f982329791fa61716e96e41e546d50f03f9b8c"

 self trade
 refnum: 5790
fname: "Jets Jets Jets"
playerid: "1352"
fillprice: 9
fillsize: 1
buyside: false
 refnum: 6035
fname: "Jets Jets Jets"
playerid: "1352"
buyside: true
size: 0
price: 12
txmetaid: "a71470373d48c7964dd4cbc4173c739f6af6254608dd4f22854c9fd3b8f78fdf"

 self trade
 refnum: 5792
fname: "Jets Jets Jets"
playerid: "1206"
fillprice: 12
fillsize: 1
buyside: false
 refnum: 6037
fname: "Jets Jets Jets"
playerid: "1206"
buyside: true
size: 0
price: 15
txmetaid: "f5520604d95257704a27979b6aacdaf572351a2fad1d18ad01542d5eba8231f8"

 self trade
 refnum: 5771
fname: "Jets Jets Jets"
playerid: "1018"
fillprice: 12
fillsize: 1
buyside: false
 refnum: 6019
fname: "Jets Jets Jets"
playerid: "1018"
buyside: true
size: 0
price: 15
txmetaid: "086ae3f5577286764ab204a2987ac71390d83da59458ef70e1546ac0af968e48"

 self trade
 refnum: 5782
fname: "Jets Jets Jets"
playerid: "1141"
fillprice: 4
fillsize: 1
buyside: false
 refnum: 6029
fname: "Jets Jets Jets"
playerid: "1141"
buyside: true
size: 0
price: 6
txmetaid: "9514e615c8b024a37abf0d5e7a10f6365d0210a2948b8818e6dbdce54e26a2aa"

 self trade
 refnum: 5779
fname: "Jets Jets Jets"
playerid: "1292"
fillprice: 11
fillsize: 1
buyside: false
 refnum: 6027
fname: "Jets Jets Jets"
playerid: "1292"
buyside: true
size: 0
price: 14
txmetaid: "61bad241c542f5940f6ce57246a44e7f56b6079500df307dc69422bb2cf38655"

 self trade
 refnum: 5769
fname: "Jets Jets Jets"
playerid: "1034"
fillprice: 8
fillsize: 1
buyside: false
 refnum: 6017
fname: "Jets Jets Jets"
playerid: "1034"
buyside: true
size: 0
price: 10
txmetaid: "3a861cbe761ce5ac32ebbf89738aa19f134f19b00ef6c7346be7261c752f3f88"

 self trade
 refnum: 5788
fname: "Jets Jets Jets"
playerid: "1284"
fillprice: 4
fillsize: 1
buyside: false
 refnum: 6033
fname: "Jets Jets Jets"
playerid: "1284"
buyside: true
size: 0
price: 6
txmetaid: "da81456bcd08f5d11c86d20b271ea093db88faac77fd82331b09a14fb0546cd0"

 self trade
 refnum: 5773
fname: "Jets Jets Jets"
playerid: "1447"
fillprice: 10
fillsize: 1
buyside: false
 refnum: 6021
fname: "Jets Jets Jets"
playerid: "1447"
buyside: true
size: 0
price: 13
txmetaid: "e7e853627640562e4a5bdfd3dc4fa8fcad688bd594c434272da29a61ae8d4e00"

 self trade
 refnum: 5784
fname: "Jets Jets Jets"
playerid: "1382"
fillprice: 6
fillsize: 1
buyside: false
 refnum: 6031
fname: "Jets Jets Jets"
playerid: "1382"
buyside: true
size: 0
price: 8
txmetaid: "0bff468a7b738f39e0b9c3e860d8f0972666287c9c9cfd31ea7b1495b6e9e955"

 self trade
 refnum: 5795
fname: "Jets Jets Jets"
playerid: "1110"
fillprice: 9
fillsize: 1
buyside: false
 refnum: 6039
fname: "Jets Jets Jets"
playerid: "1110"
buyside: true
size: 0
price: 12
txmetaid: "bd595b69fa5bdd41e57765f5b4cbcda6ea031f3895cb414537904660ca6d1db7"

 self trade
 refnum: 5815
fname: "Jets Jets Jets"
playerid: "1630"
fillprice: 3
fillsize: 1
buyside: false
 refnum: 6055
fname: "Jets Jets Jets"
playerid: "1630"
buyside: true
size: 0
price: 4
txmetaid: "47c88434b6ebe238267dbb39993227344756bf5d4a003a29853bba61b911366c"

 self trade
 refnum: 5812
fname: "Jets Jets Jets"
playerid: "1315"
fillprice: 11
fillsize: 1
buyside: false
 refnum: 6053
fname: "Jets Jets Jets"
playerid: "1315"
buyside: true
size: 0
price: 14
txmetaid: "ca45131c0945ff3ddf5361ce07036993e846b3501469e8b896702fc0cc086e23"

 self trade
 refnum: 5805
fname: "Jets Jets Jets"
playerid: "1013"
fillprice: 4
fillsize: 1
buyside: false
 refnum: 6047
fname: "Jets Jets Jets"
playerid: "1013"
buyside: true
size: 0
price: 5
txmetaid: "fad95a7b9fcdd86da025da997a7c7f3caea98d53f873e990871d797f6a395de0"

 self trade
 refnum: 5800
fname: "Jets Jets Jets"
playerid: "1584"
fillprice: 9
fillsize: 1
buyside: false
 refnum: 6043
fname: "Jets Jets Jets"
playerid: "1584"
buyside: true
size: 0
price: 12
txmetaid: "9ca074b59253943ff00721b7efb7ef59f4cece1b5d3691cb71d95e9c75bce9d7"

 self trade
 refnum: 5823
fname: "Jets Jets Jets"
playerid: "1455"
fillprice: 9
fillsize: 1
buyside: false
 refnum: 6063
fname: "Jets Jets Jets"
playerid: "1455"
buyside: true
size: 0
price: 12
txmetaid: "a4df2aa413f78a12055193c83704050086acba97070af89cc4f7b786330d0880"

 self trade
 refnum: 5798
fname: "Jets Jets Jets"
playerid: "1614"
fillprice: 4
fillsize: 1
buyside: false
 refnum: 6041
fname: "Jets Jets Jets"
playerid: "1614"
buyside: true
size: 0
price: 6
txmetaid: "28a7687ab936b2190a10445957cc9f8a94507203e02a94fee695dd5a17d859c6"

 self trade
 refnum: 5819
fname: "Jets Jets Jets"
playerid: "1167"
fillprice: 4
fillsize: 1
buyside: false
 refnum: 6059
fname: "Jets Jets Jets"
playerid: "1167"
buyside: true
size: 0
price: 5
txmetaid: "7b2e940f375d2e7b7c950577225f5d3f14896e28aa389f10d18ed903381f808a"

 self trade
 refnum: 5807
fname: "Jets Jets Jets"
playerid: "1030"
fillprice: 8
fillsize: 1
buyside: false
 refnum: 6049
fname: "Jets Jets Jets"
playerid: "1030"
buyside: true
size: 0
price: 10
txmetaid: "d61cdd2f6777a8ee4ca19b0366d816e466fb686d8b310926a0437078dbae5f86"

 self trade
 refnum: 5802
fname: "Jets Jets Jets"
playerid: "1663"
fillprice: 7
fillsize: 1
buyside: false
 refnum: 6045
fname: "Jets Jets Jets"
playerid: "1663"
buyside: true
size: 0
price: 9
txmetaid: "869751b11ed3ac49380708a77c845e3eda9259f525ab9a0fdc4eca103157d3ca"

 self trade
 refnum: 5821
fname: "Jets Jets Jets"
playerid: "1116"
fillprice: 8
fillsize: 1
buyside: false
 refnum: 6061
fname: "Jets Jets Jets"
playerid: "1116"
buyside: true
size: 0
price: 11
txmetaid: "e0cde01f787207c24815d25f75433e6157ecf0ec4d887455a89aefb0c7ee287e"

 self trade
 refnum: 5817
fname: "Jets Jets Jets"
playerid: "1346"
fillprice: 14
fillsize: 1
buyside: false
 refnum: 6057
fname: "Jets Jets Jets"
playerid: "1346"
buyside: true
size: 0
price: 17
txmetaid: "1e84031ec0044a6b369c1fdd7540a2b4a5a1c13454454c183857ebfc9c4d600c"

 self trade
 refnum: 5837
fname: "Jets Jets Jets"
playerid: "1219"
fillprice: 14
fillsize: 1
buyside: false
 refnum: 6075
fname: "Jets Jets Jets"
playerid: "1219"
buyside: true
size: 0
price: 17
txmetaid: "baa5ed1eb48e86dddf976a50ff4a68c7232e8d5b0fc36f93b508a0db4b9e2b01"

 self trade
 refnum: 5845
fname: "Jets Jets Jets"
playerid: "1832"
fillprice: 8
fillsize: 1
buyside: false
 refnum: 6083
fname: "Jets Jets Jets"
playerid: "1832"
buyside: true
size: 0
price: 10
txmetaid: "9800df9fc5e754d853fdd03d28920f2ce46aa8c8d992d04cb11881cfcca8ef92"

 self trade
 refnum: 5830
fname: "Jets Jets Jets"
playerid: "1407"
fillprice: 11
fillsize: 1
buyside: false
 refnum: 6069
fname: "Jets Jets Jets"
playerid: "1407"
buyside: true
size: 0
price: 14
txmetaid: "a4336b1a8611e53d6b70d5be8e06c9e68cf13de1c0ce95314cdac71cfeb9d961"

 self trade
 refnum: 5826
fname: "Jets Jets Jets"
playerid: "1008"
fillprice: 12
fillsize: 1
buyside: false
 refnum: 6065
fname: "Jets Jets Jets"
playerid: "1008"
buyside: true
size: 0
price: 15
txmetaid: "800d951d7b8fe3e754410e693cd8c4e05498189c122a0ca472905e17bc2ff955"

 self trade
 refnum: 5841
fname: "Jets Jets Jets"
playerid: "1126"
fillprice: 6
fillsize: 1
buyside: false
 refnum: 6079
fname: "Jets Jets Jets"
playerid: "1126"
buyside: true
size: 0
price: 8
txmetaid: "adc6cf979cde85c361c979462cafc661ae2b53dc5fc847d7870cf193a50cdb10"

 self trade
 refnum: 5833
fname: "Jets Jets Jets"
playerid: "1396"
fillprice: 5
fillsize: 1
buyside: false
 refnum: 6071
fname: "Jets Jets Jets"
playerid: "1396"
buyside: true
size: 0
price: 7
txmetaid: "4a5957e3018c843cbe769443b8aa5f93b1e9a24d8502480fa246c8da96f61eb9"

 self trade
 refnum: 5843
fname: "Jets Jets Jets"
playerid: "1420"
fillprice: 8
fillsize: 1
buyside: false
 refnum: 6081
fname: "Jets Jets Jets"
playerid: "1420"
buyside: true
size: 0
price: 10
txmetaid: "57542ae58b1676e8d1a56a12e80b5a36ad7d7521b0bff81fdf8c4da3f7173eba"

 self trade
 refnum: 5839
fname: "Jets Jets Jets"
playerid: "1092"
fillprice: 5
fillsize: 1
buyside: false
 refnum: 6077
fname: "Jets Jets Jets"
playerid: "1092"
buyside: true
size: 0
price: 7
txmetaid: "146f9281bbe8cd1428de2aa29fe926f0fa0bb1e4dc54871e124cc6b408101cce"

 self trade
 refnum: 5847
fname: "Jets Jets Jets"
playerid: "1165"
fillprice: 4
fillsize: 1
buyside: false
 refnum: 6085
fname: "Jets Jets Jets"
playerid: "1165"
buyside: true
size: 0
price: 5
txmetaid: "69adeb7ba60d24bb264c2d72d3bea306344b2b0d26897091213efafdb9ff3da2"

 self trade
 refnum: 5835
fname: "Jets Jets Jets"
playerid: "1023"
fillprice: 4
fillsize: 1
buyside: false
 refnum: 6073
fname: "Jets Jets Jets"
playerid: "1023"
buyside: true
size: 0
price: 5
txmetaid: "9174ea74541848dc8f7e7b8932615bba0c7ecfe12bcec32683079b47cca16749"

 self trade
 refnum: 5859
fname: "Jets Jets Jets"
playerid: "1118"
fillprice: 5
fillsize: 1
buyside: false
 refnum: 6097
fname: "Jets Jets Jets"
playerid: "1118"
buyside: true
size: 0
price: 7
txmetaid: "b0a2ef7291bac2d2f4ce0fd90027dbab9e33d2b36957518a048901f622c7fde9"

 self trade
 refnum: 5867
fname: "Jets Jets Jets"
playerid: "1605"
fillprice: 5
fillsize: 1
buyside: false
 refnum: 6105
fname: "Jets Jets Jets"
playerid: "1605"
buyside: true
size: 0
price: 7
txmetaid: "a28f13606e92656c9d9c7ccf92e4a8317bbe9986b6170843a770eba1f7ae0250"

 self trade
 refnum: 5865
fname: "Jets Jets Jets"
playerid: "1288"
fillprice: 6
fillsize: 1
buyside: false
 refnum: 6103
fname: "Jets Jets Jets"
playerid: "1288"
buyside: true
size: 0
price: 8
txmetaid: "464f57add8902ca0d11b8fa46e0656fee8d6eec9502275df186e774ed3c871b9"

 self trade
 refnum: 5869
fname: "Jets Jets Jets"
playerid: "1330"
fillprice: 7
fillsize: 1
buyside: false
 refnum: 6107
fname: "Jets Jets Jets"
playerid: "1330"
buyside: true
size: 0
price: 9
txmetaid: "3e0a8f4578ab5382f60cba65dd514b04ad87eb038c19053bfc7fa0315658c337"

 self trade
 refnum: 5863
fname: "Jets Jets Jets"
playerid: "1488"
fillprice: 4
fillsize: 1
buyside: false
 refnum: 6101
fname: "Jets Jets Jets"
playerid: "1488"
buyside: true
size: 0
price: 6
txmetaid: "ec7f2a4a1cc05f2bc2e20d0db4e8aef2f1c45146b4e06f543c7ba988857ec745"

 self trade
 refnum: 5855
fname: "Jets Jets Jets"
playerid: "1858"
fillprice: 2
fillsize: 1
buyside: false
 refnum: 6093
fname: "Jets Jets Jets"
playerid: "1858"
buyside: true
size: 0
price: 2
txmetaid: "e47a318f3fa0a8e316c622b63eff45aa7d8862b100f74a295ea81ea02061d144"

 self trade
 refnum: 5853
fname: "Jets Jets Jets"
playerid: "1569"
fillprice: 3
fillsize: 1
buyside: false
 refnum: 6091
fname: "Jets Jets Jets"
playerid: "1569"
buyside: true
size: 0
price: 4
txmetaid: "1c6d264dcba3a67d43f9d246100b74c007cd78b7f3bf14e1a68897870bf1e488"

 self trade
 refnum: 5857
fname: "Jets Jets Jets"
playerid: "1805"
fillprice: 4
fillsize: 1
buyside: false
 refnum: 6095
fname: "Jets Jets Jets"
playerid: "1805"
buyside: true
size: 0
price: 6
txmetaid: "50715c3752e2dbd8cd9ac06dd221f169eb7d2bc82654dca5f0bd62852acb7398"

 self trade
 refnum: 5871
fname: "Jets Jets Jets"
playerid: "1020"
fillprice: 13
fillsize: 1
buyside: false
 refnum: 6109
fname: "Jets Jets Jets"
playerid: "1020"
buyside: true
size: 0
price: 16
txmetaid: "9358c4d934a5d1cfe13ade19692c9c61756f2fbc1bca4570532fdea12d8287bf"

 self trade
 refnum: 5861
fname: "Jets Jets Jets"
playerid: "1427"
fillprice: 11
fillsize: 1
buyside: false
 refnum: 6099
fname: "Jets Jets Jets"
playerid: "1427"
buyside: true
size: 0
price: 14
txmetaid: "be8a0a6a0139464ac427bcb85f7a8eaafebddc6ad94c71e4889346d281e91c79"

 self trade
 refnum: 5850
fname: "Jets Jets Jets"
playerid: "1161"
fillprice: 9
fillsize: 1
buyside: false
 refnum: 6089
fname: "Jets Jets Jets"
playerid: "1161"
buyside: true
size: 0
price: 12
txmetaid: "4b812fe9c1f28bdbc3a2c0e89cf21c4d79bade19ddf61975282ac2f0eecadd08"

 self trade
 refnum: 5875
fname: "Jets Jets Jets"
playerid: "1406"
fillprice: 3
fillsize: 1
buyside: false
 refnum: 6113
fname: "Jets Jets Jets"
playerid: "1406"
buyside: true
size: 0
price: 4
txmetaid: "4870424fc71b586a572586581eda41c895ce36989cbc7e97dab52943bfc10529"

 self trade
 refnum: 5883
fname: "Jets Jets Jets"
playerid: "1085"
fillprice: 12
fillsize: 1
buyside: false
 refnum: 6121
fname: "Jets Jets Jets"
playerid: "1085"
buyside: true
size: 0
price: 15
txmetaid: "0e6b932c93d82dead20c0be51f1c98bccaeaa2c8ba0447d560214e6c30d46167"

 self trade
 refnum: 5877
fname: "Jets Jets Jets"
playerid: "1606"
fillprice: 11
fillsize: 1
buyside: false
 refnum: 6115
fname: "Jets Jets Jets"
playerid: "1606"
buyside: true
size: 0
price: 14
txmetaid: "e84d91ff8bf8be4e87be0e8fb41c81a12b496940b6ea968fcf791083078b5d87"

 self trade
 refnum: 5887
fname: "Jets Jets Jets"
playerid: "1357"
fillprice: 7
fillsize: 1
buyside: false
 refnum: 6125
fname: "Jets Jets Jets"
playerid: "1357"
buyside: true
size: 0
price: 9
txmetaid: "d82bd75fd1f8b7132d5bde9aba94562453e49f3d52f1fb6eb5c96276f2b20ab0"

 self trade
 refnum: 5881
fname: "Jets Jets Jets"
playerid: "1759"
fillprice: 3
fillsize: 1
buyside: false
 refnum: 6119
fname: "Jets Jets Jets"
playerid: "1759"
buyside: true
size: 0
price: 4
txmetaid: "544f231e84b2a67495ee5d8dd68a1d17416fac8ef4e0bb94dd96709fc42aba26"

 self trade
 refnum: 5873
fname: "Jets Jets Jets"
playerid: "1259"
fillprice: 4
fillsize: 1
buyside: false
 refnum: 6111
fname: "Jets Jets Jets"
playerid: "1259"
buyside: true
size: 0
price: 5
txmetaid: "eb6c799d5cc442d38385cef739e0a842ff4a981caf6338b5011cdcc0d9847df3"

 self trade
 refnum: 5879
fname: "Jets Jets Jets"
playerid: "1279"
fillprice: 9
fillsize: 1
buyside: false
 refnum: 6117
fname: "Jets Jets Jets"
playerid: "1279"
buyside: true
size: 0
price: 12
txmetaid: "335248c51c5dba443ab0e0339215406fa0796b5c2ba65d58da54105f3e2b0b45"

 self trade
 refnum: 5885
fname: "Jets Jets Jets"
playerid: "1203"
fillprice: 2
fillsize: 1
buyside: false
 refnum: 6123
fname: "Jets Jets Jets"
playerid: "1203"
buyside: true
size: 0
price: 3
txmetaid: "8911313646342ab62b07cc0eba869b116b0a1dbacbf2ed428ef79f452f9ba37b"

 self trade
 refnum: 6180
fname: "BrunoBoysErnie"
playerid: "1797"
fillprice: 8
fillsize: 5
buyside: true
 refnum: 6186
fname: "BrunoBoysErnie"
playerid: "1797"
buyside: false
size: 0
price: 8
txmetaid: "b826f5e118fbfe77978a177673450cdc79a82ee6b69c52784860c323b6d37fd3"

 self trade
 refnum: 6261
fname: "JayBNY"
playerid: "1162"
fillprice: 1
fillsize: 1
buyside: false
 refnum: 6263
fname: "JayBNY"
playerid: "1162"
buyside: true
size: 0
price: 1
txmetaid: "1d422d56cafea7cf5bb5a1e260b757f11509f808edd13fda831ad61ca2b31e50"

 self trade
 refnum: 3961
fname: "JayBNY"
playerid: "1445"
fillprice: 21
fillsize: 1
buyside: true
 refnum: 6386
fname: "JayBNY"
playerid: "1445"
buyside: false
size: 0
price: 21
txmetaid: "815bd6a8d491c71a2a427a3a26601fd109a75264ab5152e140d1a84cd90889a4"

 self trade
 refnum: 6461
fname: "JayBNY"
playerid: "1218"
fillprice: 16
fillsize: 1
buyside: true
 refnum: 6462
fname: "JayBNY"
playerid: "1218"
buyside: false
size: 0
price: 15
txmetaid: "e10be5f5b8d4affb596f2631e3d72b814d2b5ae6a0e5dd5a8af58c8b8dc4b9cf"

 self trade
 refnum: 6466
fname: "JayBNY"
playerid: "1211"
fillprice: 1
fillsize: 1
buyside: true
 refnum: 6467
fname: "JayBNY"
playerid: "1211"
buyside: false
size: 0
price: 1
txmetaid: "328e1f983ebba82c255b5076d8410de586a17e5f44a6876884a20e99c4377637"

 self trade
 refnum: 6473
fname: "JayBNY"
playerid: "10"
fillprice: 14
fillsize: 6
buyside: false
 refnum: 6474
fname: "JayBNY"
playerid: "10"
buyside: true
size: 0
price: 14
txmetaid: "fb21805349fa46cf3fc096c7b12219ba4630d735020f66e083a1d2ecbe0e2224"

 self trade
 refnum: 6446
fname: "JayBNY"
playerid: "1207"
fillprice: 1
fillsize: 1
buyside: true
 refnum: 6501
fname: "JayBNY"
playerid: "1207"
buyside: false
size: 0
price: 1
txmetaid: "b32bc2f66b49c39764106d2c968594c852e1052e159f6ce2bedc6cbab02d649a"

 self trade
 refnum: 6500
fname: "JayBNY"
playerid: "1137"
fillprice: 3
fillsize: 1
buyside: false
 refnum: 6503
fname: "JayBNY"
playerid: "1137"
buyside: true
size: 0
price: 3
txmetaid: "f22337ebba735e1fdc1f888e30e21c5376330921f975dc9ce69f039fc52f646e"

 self trade
 refnum: 3837
fname: "JayBNY"
playerid: "1212"
fillprice: 8
fillsize: 1
buyside: true
 refnum: 6505
fname: "JayBNY"
playerid: "1212"
buyside: false
size: 3
price: 8
txmetaid: "88dcd19a93ec308a48524fc1404c66509ba1c0ec1f72d092c6c2bf77a1f726e9"

 self trade
 refnum: 6443
fname: "JayBNY"
playerid: "1157"
fillprice: 19
fillsize: 1
buyside: false
 refnum: 6509
fname: "JayBNY"
playerid: "1157"
buyside: true
size: 0
price: 19
txmetaid: "b0aaa565ae1142afb96708e7dd28ab4b56df1c40962a3c4aab2219a8aa9938ac"

 self trade
 refnum: 6443
fname: "JayBNY"
playerid: "1157"
fillprice: 19
fillsize: 9
buyside: false
 refnum: 6511
fname: "JayBNY"
playerid: "1157"
buyside: true
size: 1
price: 19
txmetaid: "54335a753f6b73dd527823ccd8d8d0d729ccc08143ba74e274f1456d936fedda"

D:\work\block-explorer\release\blockexplorer.exe exited with code 0
*/
