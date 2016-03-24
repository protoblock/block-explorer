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

    std::string award(const AwardMeta &gm);

    std::unordered_map<std::string,FantasyNameBalMeta>
            m_pendingnew;

    std::unordered_map<std::string,MerkleTree>
            m_pendingawards;

    std::string award(const AwardMeta &am, const std::string &trid);
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
 * @brief The PosStore class stores positions form trading
 */
class PosStore {
public:
    std::unordered_map<std::string,PosMeta>
            m_posstatemap;

    std::unordered_map<std::string,std::string>
            m_posid2metaid;

    std::unordered_map<std::string, bool> dirtyplayerfname;

    std::vector<std::string> newposmeta;

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

        uint32_t fillqty = ofm.fillsize() * ofm.buyside() ? 1 : -1;
        uint32_t fillpos = ofm.fillprice() * -fillqty;
        auto pf = makeid(ofm.playerid(),ofm.fname());
        auto it = m_posid2metaid.find(pf);
        if ( it != m_posid2metaid.end()) {
            pm.CopyFrom(m_posstatemap[it->first]);
            pm.set_prev(it->second);
            pm.set_qty(pm.qty() + fillqty);
            pm.set_price(pm.price() + fillpos);
            dirtyplayerfname[pf] = true;
        }
        else  {
            pm.set_qty(fillqty);
            pm.set_price(fillpos);
        }

        pm.set_name(ofm.fname());

        return update(pm,pf);
    }

    std::string update(const PosMeta &pm, const std::string &pf) {
        auto newid = hashit(pm.SerializeAsString());
        m_posstatemap[newid] = pm;
        m_posid2metaid[pf] = newid;

        return newid;
    }

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

    std::unordered_map<std::string,OrderFillMeta>
            m_orderfillmetamap;

    std::vector<int32_t> m_neworders;

    std::vector<int32_t> m_dirtyorders;

    std::vector<std::string> m_newfills;
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


    std::unordered_map<std::string,MarketTicMeta>
        m_marketticmeta;

    std::unordered_map<std::string,std::string>
        m_pid2marketticid;


    std::vector<std::string> dirtyinsideid;
    std::vector<std::string> dirtypidlimitbook;
    std::vector<std::string> insideordernew;
    std::vector<std::string> dirtypid;

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

    std::string process(OrderStore &orderstore,  int32_t oref ) {
        auto ometaid = orderstore.ref2id(oref);
        auto &ometa = orderstore.m_ordermetamap[ometaid];
        auto pid = ometa.playerid();

        auto &tree = m_pid2limitbookid[ometa.playerid()];
        int newaggsize = ometa.size();

        for ( auto l : tree.leaves() ) {
            LimitBookMeta & lbm = m_limitbookidmap[l];
            auto &bapair = m_limitid2insideidpair[l];

            auto price = ometa.buyside() ? lbm.ask() : lbm.bid();

            if ( ometa.buyside() && ometa.price() < lbm.ask() ||
                 !ometa.buyside() && ometa.price() > lbm.bid()) {
                //dopassive

                insideordernew.push_back(ometaid);
                break;
            }

            else {
                //while(true) {
                auto ibmid = ometa.buyside() ? bapair.second : bapair.first;
                InsideBookMeta &ibm = m_insidemetamap[ibmid];
                int newtotal = ibm.totsize();
                std::list<std::string> &olist = m_inside2orderlist[ibm.orderidroot()];
                for (auto it = olist.begin(); it != olist.end();) {
                    auto iordid = *it;
                    OrderMeta &order = orderstore.m_ordermetamap[iordid];
                    int newpasssize = order.size();
                    int maxfills = std::min(newpasssize,newtotal);
                    if (  maxfills >= newpasssize ) {
                        //DoFill(order,iordid,order.size());
                        newtotal -= newpasssize;
                        newaggsize -= newpasssize;
                        newpasssize = 0;
                    }
                    else {
                        newpasssize -= maxfills;
                        newtotal -= maxfills;
                        //DoFill(order,iordid,newsize);
                        newaggsize -= maxfills;
                    }
                    int numfills = (order.size() - newpasssize);


                    if (numfills <= 0 ) {
                        //qDebug() << "unexpected numfilles = 0 ";
                        continue;
                    }

                    //DoFill  numfiils  order omet
                    if ( newpasssize == 0 ) olist.erase(it);
                    else it++;

                    DoFill(orderstore,order,numfills,price,iordid);

                    dirtyinsideid.push_back(ibmid);

                    if ( newtotal == 0) break;
                }

                lbm.set_bidsize(newtotal);
                dirtypidlimitbook.push_back(pid);
                DoFill(orderstore,ometa,ometa.size() - newaggsize,price,ometaid);
            }
        }

        if ( newaggsize > 0 ) {
            ometa.set_size(newaggsize);
            insideordernew.push_back(ometaid);
        }

        return "";
    }

    void DoFill(OrderStore &orderstore,const OrderMeta &om, int32_t qty,
                int32_t price,std::string oid) {
        OrderFillMeta ofm{};
        ofm.set_refnum(om.refnum());
        ofm.set_fname(om.fname());
        ofm.set_playerid(om.playerid());
        ofm.set_fillprice(price);
        ofm.set_fillsize(qty);
        ofm.set_timestamp(-1);
        ofm.set_txmetaid("txid here");
        ofm.set_buyside(om.buyside());

        MarketTicMeta mtm{} ;
        auto it = m_pid2marketticid.find(om.playerid());
        if ( it != m_pid2marketticid.end() ) {
            MarketTicMeta &prevmtm = m_marketticmeta[it->first];
            ofm.set_prev(prevmtm.orderfillhead());
            mtm.set_prev(it->second);
        }

        auto orfillmetaid = ofm.SerializeAsString();
        mtm.set_price(price);
        mtm.set_size(qty);
        mtm.set_orderfillhead(orfillmetaid);
        auto hmtms = hashit(mtm.SerializeAsString());
        m_marketticmeta[hmtms] = mtm;
        m_pid2marketticid[om.playerid()] = hmtms;
        dirtypid.push_back(om.playerid());

        orderstore.m_newfills.push_back(orfillmetaid);
        orderstore.m_orderfillmetamap[orfillmetaid] = ofm;

        OrderMeta omn;
        omn.CopyFrom(om);
        omn.set_prev(oid);
        omn.set_orderfillmetaid(orfillmetaid);
        omn.set_size(om.size() - qty);
        omn.set_price(price);
        omn.set_prev(oid);

        auto newid = hashit(omn.SerializeAsString());
        orderstore.m_ordermetamap[newid] = omn;
        orderstore.m_refnum2orderid[om.refnum()] = newid;
        orderstore.m_dirtyorders.push_back(om.refnum());
        //ofm.set_prev();

        PlayerMarketState pmm{};
        auto pit = m_pid2marketid.find(om.playerid());
        if ( pit != m_pid2marketid.end()) {
            pmm.CopyFrom(m_marketmetamap[pit->second]);
            pmm.set_prev(pit->second);
        }
//        pmm.set_limitbookmetaid();
        pmm.set_playerid(om.playerid());
        pmm.set_marketticmetaid(hmtms);
        pmm.set_limitbookmetaid(m_pid2limitbookid[om.playerid()].root());
        auto pmmid = hashit(pmm.SerializeAsString());
        m_marketmetamap[pmmid] = pmm;
        m_pid2marketid[om.playerid()] = pmmid;
    }

    void update(OrderStore &orderstore) {
        //std::map<std::string,std::vector<LimitBookMeta>> newlimitsmap;
        for ( auto &newoid : insideordernew ) {
            OrderMeta &ordernew = orderstore.m_ordermetamap[newoid];
            auto &tree = m_pid2limitbookid[ordernew.playerid()];

            bool workingask = false;
            bool workingbid = false;

            LimitBookMeta lbm2;
            std::vector<LimitBookMeta> newlimit;// = newlimitsmap[ordernew.playerid()];

//            if ( newlimits.size() == 0) {
//                for ( int i=0; i < tree.leaves().size(); i++) {
//                    newlimits.push_back(m_limitbookidmap[tree.leaves(i)]);
//                }
//            }
//            if ( ordernew.buyside()) {
//                LimitBookMeta &lbm = newlimits[40-ordernew.price()-1];
//                lbm.set_bidsize(lbm.bidsize() + dordernew.size());
//                lbm.set_bid(ordernew.price());
//            }
//            if ( !ordernew.buyside()) {
//                LimitBookMeta &lbm = newlimits[40-ordernew.price()];
//                lbm.set_askize(lbm.asksize() + dordernew.size());
//                lbm.set_ask(ordernew.price());
//            }


            for ( int i = 0; i < tree.leaves_size(); i++) {
                LimitBookMeta lbm = m_limitbookidmap[tree.leaves(i)];
                //if ( ordernew.buyside() ) {
                    if ( workingask ) {
                        lbm2.set_ask(lbm.ask());
                        lbm2.set_asksize(lbm.asksize());
                        newlimit.push_back(lbm2);
                        lbm2.Clear();
                        if (lbm.bidsize() != 0) {
                            lbm2.set_bid(lbm.bid());
                            lbm2.set_bidsize(lbm.bidsize());
                            workingbid = true;
                            continue;
                        }
                        //workingask = false;
                        //if (!workingbid) break;
                    }
                    else if ( workingbid ) {
                        lbm2.set_bid(lbm.bid());
                        lbm2.set_bidsize(lbm.bidsize());
                        newlimit.push_back(lbm2);
                        lbm2.Clear();
                        if (lbm.asksize() != 0) {
                            lbm2.set_ask(lbm.ask());
                            lbm2.set_asksize(lbm.asksize());
                            workingask = true;
                            continue;
                        }
                        //workingbid = false;
                        //if (!workingask) break;
                    }
                    else if ( ordernew.buyside() && lbm.bid() < ordernew.price()) {
                        lbm2.set_bid(ordernew.price());
                        lbm2.set_bidsize(ordernew.size());
                        lbm2.set_ask(lbm.ask());
                        lbm2.set_asksize(lbm.asksize());
                        newlimit.push_back(lbm2);
                        lbm2.Clear();
                        if ( lbm.bidsize() != 0 ) {
                            lbm2.set_bid(lbm.bid());
                            lbm2.set_bidsize(lbm.bidsize());
                            workingask = true;
                        }
                    }
                    else if ( ordernew.buyside() && lbm.bidsize() == 0 && lbm.bid() < ordernew.price()) {
                        lbm2.set_bid(ordernew.price());
                        lbm2.set_bidsize(ordernew.size());
                        lbm2.set_ask(lbm.ask());
                        lbm2.set_asksize(lbm.asksize());
                    }
                    else if ( ordernew.buyside() && lbm.bidsize() > 0 && lbm.bid() == ordernew.price()) {
                        lbm2.set_bid(ordernew.price());
                        lbm2.set_bidsize(ordernew.size());
                        lbm2.set_ask(lbm.ask());
                        lbm2.set_asksize(lbm.asksize());
                    }

                    else if ( !ordernew.buyside() && lbm.ask() > ordernew.price()) {
                        lbm2.set_ask(ordernew.price());
                        lbm2.set_asksize(ordernew.size());
                        lbm2.set_bid(lbm.bid());
                        lbm2.set_bidsize(lbm.bidsize());
                        newlimit.push_back(lbm2);
                        lbm2.Clear();
                        if ( lbm.asksize() != 0 ) {
                            lbm2.set_ask(lbm.ask());
                            lbm2.set_asksize(lbm.asksize());
                            workingbid = true;
                        }
                    }
                    else if ( ordernew.buyside() && lbm.bidsize() == 0 && lbm.bid() < ordernew.price()) {
                        lbm2.set_bid(ordernew.price());
                        lbm2.set_bidsize(ordernew.size());
                        lbm2.set_ask(lbm.ask());
                        lbm2.set_asksize(lbm.asksize());
                    }

                    else
                        newlimit.push_back(lbm);
                        \
                }
            }
    }

//    void DoFill()
};

}

#endif // DATASTORES_H
