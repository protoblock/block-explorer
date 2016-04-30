#ifndef DATASTORES_H
#define DATASTORES_H

//#include "createstate.h"
#include "unordered_map"
#include "proto/StateData.pb.h"
#include "utils.h"
#include <set>
#include <list>
#include <QMap>
#include <QSharedPointer>
#include <QString>
#include "limitbook.h"

namespace fantasybit_bx {

using namespace fantasybit;
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

    //std::string award(const AwardMeta &gm);

    std::unordered_map<std::string,FantasyNameBalMeta>
            m_pendingnew;

    std::unordered_map<std::string,MerkleTree>
            m_pendingawards;

    std::unordered_map<std::string,MerkleTree>
            m_pendingpnl;

    std::string award(const AwardMeta &am, const std::string &trid);
    std::string pnl(const PnlMeta &am, const std::string &trid);

    uint32_t getStake(const std::string &fname) {
        auto it = m_name2metaid.find(fname);
        if ( it == end(m_name2metaid)) return 0;

        auto iit = m_fantasynamebalmetamap.find(it->second);
        if ( iit == end(m_fantasynamebalmetamap)) return 0;

        return iit->second.bits();
    }

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

    std::unordered_map<std::string,std::string>
            m_id2gameresultmeta;

    GameStatusMeta getGameStatusMeta(const std::string &gameid) {
        return m_gamestatsstatemap[m_gameid2metaid[gameid]];
    }


    void init();

    std::string process(const std::string &dataid,
                        const ScheduleData &sd);

    std::string process(const std::string &dataid,
                        const GameData &gd);

    std::string process(const std::string &dataid,
                        const ResultData &gd);

    std::string update(const GameStatusMeta &gm);

    std::string close(const std::string &dataid,
                        const std::string &id);

    std::map<int, std::vector<MerkleTree>>
                        createGameStatusmetaidroots();

    std::string start(const std::string &gmid, const GameMeta &gm);

    void clean();

    void addInGameProjMeta(const std::string &gameid, int week, const std::string &ingameprojid);

    void addGameResultMeta(const std::string &gameid, int week, const std::string &resultid);

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

    std::set<std::string> newprojmeta;

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

    void clear() {
        m_projstatemap.clear();
        m_projid2metaid.clear();
        dirtyplayerfname.clear();
        newprojmeta.clear();
        dirty = false;
    }
};

/**
 * @brief The PosStore class stores positions form trading
 */
class PosStore {
public:
    std::unordered_map<std::string,PosMeta>
            m_posstatemap;

    std::unordered_map<std::string,std::string>
            m_posid2metaid;

    std::unordered_map<std::string, bool> dirtyplayerfname;

    std::set<std::string> newposmeta;

    void init() {
        for ( auto pmp : m_posstatemap) {
            auto id = makeid(pmp.second.playerid(),pmp.second.name());
            m_posid2metaid[id] = pmp.first;
            //dirtyplayerfname[id] = false;
        }
        dirtyplayerfname.clear();
        dirty = false;
    }

    static std::string makeid(const std::string &pid, const std::string &fn) {
        return pid + ":" + fn;
    }

//    std::string process(const std::string &txid,
//                        const ProjectionTrans &pj,
//                        const std::string &fname);

    std::string process(const OrderFillMeta &ofm) {
        PosMeta pm;

        uint32_t fillqty = ofm.fillsize() * (ofm.buyside() ? 1 : -1);
        uint32_t fillpos = ofm.fillprice() * -fillqty;
        auto pf = makeid(ofm.playerid(),ofm.fname());
        auto it = m_posid2metaid.find(pf);
        if ( it != m_posid2metaid.end()) {
            pm.set_prev(it->second);
            pm.CopyFrom(m_posstatemap[it->second]);
            pm.set_qty(pm.qty() + fillqty);
            pm.set_price(pm.price() + fillpos);
            if ( newposmeta.find(pf) != newposmeta.end() )
                m_posstatemap.erase(it->second);
            else if ( dirtyplayerfname.find(pf) !=  dirtyplayerfname.end())
                m_posstatemap.erase(it->second);
            else
                dirtyplayerfname[pf] = true;
        }
        else {
            newposmeta.insert(pf);
            pm.set_qty(fillqty);
            pm.set_price(fillpos);
            pm.set_name(ofm.fname());
            pm.set_playerid(ofm.playerid());
        }

        pm.set_txmetaid(ofm.txmetaid());

        return update(pm,pf);
    }

    std::string update(const PosMeta &pm, const std::string &pf) {
        auto newid = hashit(pm.SerializeAsString());
        m_posstatemap[newid] = pm;
        m_posid2metaid[pf] = newid;

        return newid;
    }

    bool dirty = false;

    void clear() {
        m_posstatemap.clear();
        m_posid2metaid.clear();
        dirtyplayerfname.clear();
        newposmeta.clear();
        dirty = false;
    }

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

    std::unordered_map<std::string,OrderFillMeta>
            m_orderfillmetamap;

    std::unordered_map<int32_t,MerkleTree>
            m_orderFills;

    std::vector<int32_t> m_neworders;

    std::set<int32_t> m_dirtyorders;

    std::vector<std::string> m_newfills;

    std::set<std::string> m_dirtypid;

    std::set<std::string> m_dirtypidfname;

    void clear() {
        m_ordermetamap.clear();
        m_orderfillmetamap.clear();
        m_orderFills.clear();
    }

//    std::unordered_map<std::string, bool> dirtyplayerfname;
//    std::vector<std::string> newprojmeta;
    void init() {
        for ( auto om : m_ordermetamap) {
            m_refnum2orderid[om.second.refnum()] = om.first;
        }
    }

    std::string ref2id(int32_t iref)  {
        return m_refnum2orderid.at(iref);
    }

    OrderMeta &ref2OrderMeta(int32_t iref)  {
        return   m_ordermetamap[this->ref2id(iref)] ;
    }

    std::string newFill(const OrderFillMeta &fill) {

        const auto &it = m_orderFills.find(fill.refnum());
        if ( it == m_orderFills.end())
            m_orderFills[fill.refnum()] = {};

        MerkleTree &mtree = m_orderFills[fill.refnum()];
        auto hf = hashit(fill);
        mtree.add_leaves(hf);

        auto &om = ref2OrderMeta(fill.refnum());
        om.set_size(om.size()-fill.fillsize());
        if (m_dirtyorders.find(om.refnum()) == m_dirtyorders.end()) {
            om.set_prev(ref2id(om.refnum()));
            m_dirtyorders.insert(om.refnum());
        }
        m_newfills.push_back(hf);
        return hf;
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

    std::string process_cancel(const std::string &txid,
                            const ExchangeOrder &eo,
                            const std::string &fname,
                            int32_t refnum) {


        auto it = m_refnum2orderid.find(eo.cancel_oref());
        if ( it == end(m_refnum2orderid))
            return "";

        OrderMeta &om = m_ordermetamap[it->second];
        om.set_prev(it->second);
        om.set_size(0);
        om.set_txmetaid(txid);
        auto oid = hashit(om.SerializeAsString());
        m_ordermetamap[oid] = om;
        m_refnum2orderid[refnum] = oid;
        m_dirtyorders.insert(om.refnum());
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
    QMap<QString,QSharedPointer<LimitBook>> m_pid2LimitBook;

    std::unordered_map<std::string,PlayerMarketState>
        m_marketmetamap;

    std::unordered_map<std::string,std::string>
        m_pid2marketid;

    std::unordered_map<std::string,LimitBookMeta>
        m_limitbookidmap;

    std::unordered_map<std::string,MarketTicMeta>
        m_marketticmeta;

    std::unordered_map<std::string,std::string>
        m_pid2marketticid;

    bool dirty = false;

    void clear() {
        m_pid2LimitBook.clear();
        m_marketmetamap.clear();
        m_limitbookidmap.clear();
        m_marketticmeta.clear();
        m_pid2marketticid.clear();
        m_pid2marketid.clear();
        dirty = false;
    }

    void init() {
        for ( auto mm : m_marketmetamap) {
            m_pid2marketid[mm.second.playerid()] = mm.first;
            if ( mm.second.playerid() == "1619") {
                int i;
                i++;
            }

        }

//        for ( auto io : m_inside2ordertree) {
//            std::list<std::string> olist;
//            for (int i = 0;i < io.second.leaves_size();i++) {
//                olist.push_back(io.second.leaves(i));
//            }
//            if ( olist.size() > 0) {
//                m_inside2orderlist[io.first] = olist;
//                olist.clear();
//            }
//        }

    }

    bool isLocked(const std::string &pid) {
        auto it = m_pid2marketid.find(pid);
        if ( it == end(m_pid2marketid) )
            return false;

        auto &pms = m_marketmetamap[it->second];
        return pms.locked();
    }

    std::string lock(const std::string &pid) {
       std::string ret;
       auto it = m_pid2marketid.find(pid);
       if ( it == m_pid2marketid.end() ) {
          PlayerMarketState pms;
          pms.set_locked(true);
          pms.set_playerid(pid);
          ret = hashit(pms);
          m_marketmetamap[ret] = pms;
       }
       else {
          PlayerMarketState &pms = m_marketmetamap[pid];
          pms.set_prev(it->second);
          pms.set_locked(true);
          ret = hashit(pms);
          m_marketmetamap.erase(it->second);
       }
       m_pid2marketid[pid] = ret;

       return ret;
    }

    void addLimitbook(const QString &playerid) {
        QSharedPointer<LimitBook> mylb{new LimitBook(playerid)};
        m_pid2LimitBook.insert(playerid,mylb)  ;

    }

    std::string newFill(OrderFillMeta &fill) {
        MarketTicMeta mtm{} ;
        auto it = m_pid2marketticid.find(fill.playerid());
        if ( it != m_pid2marketticid.end() ) {
           MarketTicMeta &prevmtm = m_marketticmeta[it->first];
           fill.set_prev(prevmtm.orderfillhead());
           mtm.set_prev(it->second);
        }
        mtm.set_price(fill.fillprice());
        mtm.set_size(fill.fillsize());
        mtm.set_orderfillhead(hashit(fill));
        auto mid = hashit(mtm);
        m_pid2marketticid[fill.playerid()] = mid;
        m_marketticmeta[mid] = mtm;
        return mid;
    }

};
}

#endif // DATASTORES_H
