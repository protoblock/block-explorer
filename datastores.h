#ifndef DATASTORES_H
#define DATASTORES_H

//#include "createstate.h"
#include "unordered_map"
#include "proto/StateData.pb.h"
#include "utils.h"

namespace fantasybit {


/**
 * @brief The PlayerStore class stores playermeta state data
 */
class PlayerStore {
public:
    std::unordered_map<std::string,PlayerMeta>
            m_playerstatemap;

    std::unordered_map<std::string,std::string>
            m_playerid2metaid;

    std::map<std::string,std::string>
            m_playerid2teamid;

    std::unordered_map<std::string, bool> dirtyeteam;

    void init();

    std::string process(const std::string &dataid, const PlayerData &pd);

    std::string update(const PlayerMeta &pm, const std::string &prevteam);

    std::unordered_map<std::string,
                       fantasybit::MerkleTree> createPlayermetaidroots();

    void clean();
};

/**
 * @brief The FantasyNameStore class stored gamestatusmeta state data
 */
class FantasyNameStore {
public:

    std::unordered_map<std::string,FantasyNameBalMeta>
            m_fantasynamebalmetamap;

    std::unordered_map<std::string,std::string>
            m_name2metaid;

    std::vector<std::string> dirty;

    void init();

    std::string process_new(const std::string &txid,
                        const NameTrans &nt);

    std::string update(const FantasyNameBalMeta &gm);
};

/**
 * @brief The GameStatusStore class stored gamestatusmeta state data
 */
class GameStatusStore {
public:
    std::unordered_map<std::string, GameStatusMeta>
            m_gamestatsstatemap;

    std::unordered_map<std::string, std::string>
            m_gameid2metaid;

    std::map<std::string, int>
            m_gameid2week;

    std::unordered_map<int, bool> dirtyweek;

    bool dirty = false;

    std::unordered_map<std::string,std::string>
            m_id2ingameprojmeta;

    GameStatusMeta getGameStatusMeta(const std::string &gameid) {
        return m_gamestatsstatemap[m_gameid2metaid[gameid]];
    }

    void init();

    std::string process(const std::string &dataid,
                        const ScheduleData &sd);

    std::string process(const std::string &dataid,
                        const GameData &gd);

    std::string update(const GameStatusMeta &gm);

    std::string close(const std::string &dataid,
                        const std::string &id);

    std::unordered_map<int, std::vector<MerkleTree>>
                        createGameStatusmetaidroots();

    std::string start(const std::string &gmid, const GameMeta &gm);

    void clean();

    void addInGameProjMeta(const std::string &gameid, int week, const std::string &ingameprojid);
};

/**
 * @brief The ProjStore class stores projmeta state data
 */
class ProjStore {
public:
    std::unordered_map<std::string,ProjMeta>
            m_projstatemap;

    std::unordered_map<std::string,std::string>
            m_projid2metaid;

    std::unordered_map<std::string, bool> dirtyplayerfname;

    std::vector<std::string> newprojmeta;

    void init();

    static std::string makeid(const std::string &pid, const std::string &fn) {
        return pid + ":" + fn;
    }

    std::string process(const std::string &txid,
                        const ProjectionTrans &pj,
                        const std::string &fname);

    std::string process(const std::string &txid,
                        const PlayerPoints &pj,
                        const std::string &fname);

    std::string update(const ProjMeta &pm, const std::string &pf);

    bool dirty = false;
};

/**
 * @brief The ProjStore class stores projmeta state data
 */
class OrderStore {
public:
    std::unordered_map<std::string,OrderMeta>
            m_ordermetamap;

    std::unordered_map<int32_t,std::string>
            m_refnum2orderid;

    std::vector<int32_t> m_neworders;
//    std::unordered_map<std::string, bool> dirtyplayerfname;

//    std::vector<std::string> newprojmeta;

    void init() {
        for ( auto om : m_ordermetamap) {
            m_refnum2orderid[om.second.refnum()] = om.first;
        }
    }

    std::string process_new(const std::string &txid,
                            const ExchangeOrder &eo,
                            const std::string &fname,
                            int32_t refnum) {
        OrderMeta om{};
        om.set_refnum(refnum);
        om.set_buyside(eo.core().buyside());
        om.set_fname(fname);
        om.set_playerid(eo.playerid());
        om.set_size(eo.core().size());
        om.set_price(eo.core().price());
        om.set_txmetaid(txid);

        auto oid = hashit(om.SerializeAsString());
        m_ordermetamap[oid] = om;
        m_refnum2orderid[refnum] = oid;
        m_neworders.push_back(refnum);
        return oid;
    }
//        optional int32 refnum = 1;
//        optional string fname = 10;
//        optional string playerid = 20;
//        optional bool buyside = 30;
//        optional int32 size = 40;
//        optional int32 price = 50;
//        optional bytes txmetaid = 60;
//        optional bytes orderfillmetaroot = 70;
//        optional bytes prev = 80;

//    std::string process(const std::string &txid,
//                        const PlayerPoints &pj,
//                        const std::string &fname);

//    std::string update(const ProjMeta &pm, const std::string &pf);

//    bool dirty = false;
};

class MarketStore {
public:
    std::unordered_map<std::string,PlayerMarketState>
        m_marketmetamap;

    std::unordered_map<std::string,std::string>
        m_pid2marketid;

//    std::unordered_map<std::string, std::string[40]>
    std::unordered_map<std::string, MerkleTree>
        m_pid2limitbookid;

    std::unordered_map<std::string,LimitBookMeta>
        m_limitbookidmap;

    std::unordered_map<std::string,InsideBookMeta>
        m_insidemetamap;

    std::unordered_map<std::string,std::pair<std::string,std::string>>
        m_limitid2insideidpair;

    std::unordered_map<std::string,MerkleTree>
        m_inside2ordertree;

    std::unordered_map<std::string,std::list<std::string>>
        m_inside2orderlist; //ToDo

    void init() {
        for ( auto mm : m_marketmetamap) {
            m_pid2marketid[mm.second.playerid()] = mm.first;
        }

        for ( auto io : m_inside2ordertree) {
            std::list<std::string> olist;
            for (int i = 0;i < io.second.leaves_size();i++) {
                olist.push_back(io.second.leaves(i));
            }
            if ( olist.size() > 0) {
                m_inside2orderlist[io.first] = olist;
                olist.clear();
            }
        }

    }

};

}

#endif // DATASTORES_H
