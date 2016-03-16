#ifndef CREATESTATE_H
#define CREATESTATE_H
#include <string>
#include "proto/StateData.pb.h"
#include <fc/crypto/sha256.hpp>
#include "ldbwriter.h"
#include "utils.h"
#include "unordered_map"

namespace fantasybit {


class PlayerStore {
public:
    std::unordered_map<std::string,PlayerMeta>
            m_playerstatemap;

    std::unordered_map<std::string,std::string>
            m_playerid2metaid;

    std::map<std::string,std::string>
            m_playerid2teamid;

    std::unordered_map<std::string, bool> dirtyeteam;

    void init() {
        for ( auto pmp : m_playerstatemap) {
           m_playerid2metaid[pmp.second.playerid()] = pmp.first;
           std::string teamid = pmp.second.player_status().teamid();
           if ( teamid == "")
               teamid = "FA";
           m_playerid2teamid[pmp.second.playerid()] = teamid;
           if ( dirtyeteam.find(teamid) == dirtyeteam.end())
               dirtyeteam[teamid] = false;
        }
    }

    std::string process(const std::string &dataid, const PlayerData &pd) {
        std::string prevteam = "";
        PlayerMeta pm;
        auto it = m_playerid2metaid.find(pd.playerid());
        if ( it != m_playerid2metaid.end()) {
            pm.CopyFrom(m_playerstatemap[it->second]);
            pm.set_prev(it->second);
            prevteam = m_playerid2teamid[pd.playerid()];
        }
        else pm.set_playerid(pd.playerid());

        pm.set_datametaid(dataid);
        if ( pd.has_player_base())
            pm.mutable_player_base()->CopyFrom(pd.player_base());

        if ( pd.has_player_status()) {
            if ( pm.has_player_status() ) {
                if ( pd.player_status().has_status())
                    pm.mutable_player_status()->set_status(pd.player_status().status());
                if ( pd.player_status().has_teamid())
                    pm.mutable_player_status()->set_teamid(pd.player_status().teamid());
            }
            else
                pm.mutable_player_status()->CopyFrom(pd.player_status());
        }

        return update(pm,prevteam);
    }

    std::string update(const PlayerMeta &pm, const std::string &prevteam) {
        auto newid = hashit(pm.SerializeAsString());
        m_playerstatemap[newid] = pm;
        m_playerid2metaid[pm.playerid()] = newid;
        std::string teamid = pm.player_status().teamid();
        if ( teamid == "")
            teamid = "FA";

        m_playerid2teamid[pm.playerid()] = teamid;
        dirtyeteam[teamid] = true;
        if ( prevteam != "")
            dirtyeteam[prevteam] = true;

        return newid;
    }
};

class GameStatusStore {
public:

    std::unordered_map<std::string,GameStatusMeta>
            m_gamestatsstatemap;

    std::unordered_map<std::string,std::string>
            m_gameid2metaid;

    std::map<std::string,int>
            m_gameid2week;

    std::unordered_map<int, bool> dirtyweek;

    void init() {
        for ( auto gmp : m_gamestatsstatemap) {
           m_gameid2metaid[gmp.second.id()] = gmp.first;
           m_gameid2week[gmp.second.id()] = gmp.second.week();
           if ( dirtyweek.find(gmp.second.week()) == dirtyweek.end())
               dirtyweek[gmp.second.week()] = false;
        }
    }

    std::string process(const std::string &dataid,
                        const ScheduleData &sd) {
        dirtyweek[sd.week()] = true;
        GameStatus gs;
        gs.set_status(GameStatus_Status_SCHEDULED);
        for (auto games : sd.weekly().games()) {
            GameStatusMeta gsm;
            gsm.set_id(games.id());
            gsm.set_week(sd.week());
            gsm.mutable_gameinfo()->CopyFrom(games);
            gs.set_datetime(games.time());
            gsm.mutable_gamesatus()->CopyFrom(gs);
            gsm.set_datametaid(dataid);
            auto newid = hashit(gsm.SerializeAsString());
            m_gamestatsstatemap[newid] = gsm;
            m_gameid2metaid[gsm.id()] = newid;
            m_gameid2week[gsm.id()] = sd.week();
        }

        return "";
    }

    std::string process(const std::string &dataid,
                        const GameData &gd) {
        GameStatusMeta gm;
        auto it = m_gameid2metaid.find(gd.gameid());
        if ( it == m_gameid2metaid.end())
            return "";

        gm.CopyFrom(m_gamestatsstatemap[it->second]);
        gm.set_prev(it->second);
        gm.mutable_gamesatus()->set_datetime(gd.status().datetime());
        gm.set_datametaid(dataid);

        return update(gm);
    }

    std::string update(const GameStatusMeta &gm) {
        auto newid = hashit(gm.SerializeAsString());
        m_gamestatsstatemap[newid] = gm;
        m_gameid2metaid[gm.id()] = newid;

        dirtyweek[gm.week()] = true;

        return newid;
    }

    std::string close(const std::string &dataid,
                        const std::string &id) {
        GameStatusMeta gm;
        auto it = m_gameid2metaid.find(id);
        if ( it == m_gameid2metaid.end())
            return "";

        gm.CopyFrom(m_gamestatsstatemap[it->second]);
        gm.set_prev(it->second);
        gm.mutable_gamesatus()->set_status(GameStatus::CLOSED);
        gm.set_datametaid(dataid);

        return update(gm);
    }

};


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
//      optional bytes teamstateid = 70; //teammetaroot
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

    /**
     * @brief process blockmeta in order
     *      1. transition data
     *      2. name transactions
     *      3. other transactions
     *      4. transition
     * @param bm
     */
    void process(const BlockMeta &bm) {
        TrMeta trm;
        ldb.read(bm.trmetaid(),trm);
        processTrData(trm.datametaroot());
        processTx(bm.txmetaroot());
        processTr(trm);
    }

    void processTrData(const std::string &datametaroot) {
        MerkleTree datametatree;
        std::unordered_map<std::string, DataMeta> datametamap;
        loadMerkleMap(datametaroot,datametatree,datametamap);

        std::string id;
        ResultData rd;
        for(auto datap : datametamap) {
            switch(datap.second.data().type()) {
            case Data_Type_PLAYER:
                id = m_playerstore.process(datap.first,
                            datap.second.data().GetExtension(PlayerData::player_data));
                if ( id != "")
                    ldb.write(id,m_playerstore.m_playerstatemap[id].SerializeAsString());
                break;
            case Data_Type_SCHEDULE:
                m_gamestatustore.process(datap.first,
                            datap.second.data().GetExtension(ScheduleData::schedule_data));
                break;

            case Data_Type_GAME:
                id = m_gamestatustore.process(datap.first,
                            datap.second.data().GetExtension(GameData::game_data));
                if ( id != "")
                    ldb.write(id,m_gamestatustore.m_gamestatsstatemap[id].SerializeAsString());

                break;
            case Data_Type_RESULT:
                rd = datap.second.data().GetExtension(ResultData::result_data);
                id = m_gamestatustore.close(datap.first,rd.game_result().gameid());
                if ( id != "")
                    ldb.write(id,m_gamestatustore.m_gamestatsstatemap[id].SerializeAsString());
                //ToDo:

                break;
            default:
                break;
            }
        }

    }

    void processTr(const TrMeta &trmeta) {}

    void processTx(const std::string &txmetaid) {
        MerkleTree txtree;
        std::unordered_map<std::string, TxMeta> txtreemap;
        loadMerkleMap(txmetaid,txtree,txtreemap);

        processNameTx(txtreemap);
        processRegTx(txtreemap);
    }

    void processNameTx(std::unordered_map<std::string, TxMeta> &tmap) {
        for ( auto nt : tmap ) {
            if ( nt.second.txtype() != TransType::NAME)
                continue;
        }
    }

    void processRegTx(std::unordered_map<std::string, TxMeta> &tmap) {
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

    PlayerStore m_playerstore;
    GameStatusStore m_gamestatustore;

    static std::vector<std::string> GENESIS_NFL_TEAMS;

};


}
#endif // CREATESTATE_H
