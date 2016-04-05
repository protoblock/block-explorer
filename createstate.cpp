#include "createstate.h"
#include "datastores.h"
#include "DistributionAlgo.h"

using namespace std;
namespace fantasybit {

void CreateState::dump(const std::string &pbstateid) {
    //m_pbstateid = loadStateId(pbstateid);

    //qDebug() << "test" << m_pbstate.DebugString().data();
    dumpMerkleMap(m_weekgamestatusmetamap);
}

std::string CreateState::createState(const BlockMeta &bm) {

    std::string pbstatemetaid = getStateId(bm.prev());

      if ( pbstatemetaid == "")
        loadDefaultStates();
    else if ( pbstatemetaid != m_pbstateid) {
        m_pbstateid = loadStateId(pbstatemetaid);
        loadSubStates();
    }

//    if ( bm.pbstateid() != m_pbstateid ) {
//        return "error"; //ToDo:
//    }

    //dump("");
    process(bm);
    //dump("");

    auto pbstatestr =  m_pbstate.SerializeAsString();
    auto pbstateid = fc::sha256::hash(pbstatestr).str();

    ldb.write(m_pbstate);
    m_pbstateid = pbstateid;
    return pbstateid;
}

std::string fantasybit::CreateState::loadStateId(const std::string &pbstatemetaid) {

    auto pbstatemetastr = ldb.read(pbstatemetaid);
    if ( !m_pbstate.ParseFromString(pbstatemetastr) )
        return "";

    this->loadMerkleMap(m_pbstate.schedulestateid(),
                  m_weekgamestatusmetatree,
                  m_weekgamestatusmetamap);

    ldb.read(m_pbstate.leaderboardstateid(),m_leaderboardmeta);

    m_projmetamap.clear();
    m_projmetatree.Clear();
    this->loadMerkleMap(m_pbstate.projstateid(),
                  m_projmetatree,
                  m_projmetamap);

    this->loadMerkleMap(m_pbstate.orderstateid(),
                  m_ordermetatree,
                  m_ordermetamap);

    this->loadMerkleMap(m_pbstate.marketstateid(),
                  m_playermarketstatetree,
                  m_playermarketstatemap);

    ldb.read(m_pbstate.globalstateid(),m_globalstatemeta);

    this->loadMerkleMap(m_pbstate.teamstatemid(),
                  m_teamstatetree,
                  m_teamstatemap);

    this->loadMerkleMap(m_pbstate.fantasynamestateid(),
                  m_fantasynamestatetree,
                  m_fantasynamestatemap);

    return hashit(pbstatemetastr);
}

std::string fantasybit::CreateState::loadDefaultStates() {

    TeamMeta tm;
    for( auto d : CreateState::GENESIS_NFL_TEAMS) {
        tm.set_teamid(d);
        auto htm = ldb.write(tm);
        m_teamstatetree.add_leaves(htm);
        m_teamstatemap[htm] = tm;
    }
    m_teamstatetree.set_root(makeMerkleRoot(m_teamstatetree.leaves()));
    m_pbstate.set_teamstatemid(m_teamstatetree.root());

//    GlobalState gs;
//    gs.set_week(1);
//    gs.set_season(2015);
//    m_globalstatemeta.mutable_globalstate()->CopyFrom(gs);
//    m_pbstate.set_globalstateid(ldb.write(m_globalstatemeta));

    m_pbstateid = hashit(m_pbstate);

    return m_pbstateid;
}


std::string fantasybit::CreateState::getStateId(const std::string &blockmetaid) {
    std::string ret = "";

    auto blockmetastr = ldb.read(blockmetaid);
    BlockMeta bm;
    if ( bm.ParseFromString(blockmetastr) )
        ret = bm.pbstateid();

    return ret;
}

void CreateState::loadSubStates() {
    loadPlayerState();
    loadGameState();
    loadFantasyNameBalState();
    loadProjState();
    loadOrderState();
}

void CreateState::loadOrderState() {
    m_orderstore.m_ordermetamap.insert(m_ordermetamap.begin(),
                                       m_ordermetamap.end());

    m_orderstore.init();

    m_marketstore.m_marketmetamap.insert(m_playermarketstatemap.begin(),
                                  m_playermarketstatemap.end());

    m_marketstore.init();

    for ( auto &pms : m_playermarketstatemap) {
        MerkleTree mtree{};
        ldb.read(pms.second.limitbookmetaid(),mtree);
        this->loadMerkleMap(pms.second.limitbookmetaid(),
                            mtree,
                            m_marketstore.m_limitbookidmap);

//        m_marketstore.m_pid2limitbookid[pms.second.playerid()] =
//                mtree;

        QString pid = pms.second.playerid().data();
        m_marketstore.addLimitbook(pid);
        auto lb = m_marketstore.m_pid2LimitBook[pid];
        for ( auto &lbid : mtree.leaves()) {
            LimitBookMeta &lbm = m_marketstore.m_limitbookidmap[lbid];

//            m_marketstore.m_insidemetamap
//                    [lbm.insidebookmetabid()] = ibmb;

//            m_marketstore.m_inside2ordertree[lbm.insidebookmetabid()] = imtree;

            MerkleTree imtree{};
            ldb.read(lbm.bidordermetaroot(),imtree);
            InsideBook *ib = lb->getInside(true,lbm.bid());
            for ( auto &omid : imtree.leaves()) {
                OrderMeta om{};
                ldb.read(omid,om);
                ib->New(om);
            }

            imtree.Clear();
            ldb.read(lbm.askordermetaroot(),imtree);
            ib = lb->getInside(true,lbm.ask());
            for ( auto &omid : imtree.leaves()) {
                OrderMeta om{};
                ldb.read(omid,om);
                ib->New(om);
            }

        }

//        for ( auto &lbid : mtree.leaves()) {
//            LimitBookMeta &lbm = m_marketstore.m_limitbookidmap[lbid];
//            InsideBookMeta ibmb{};
//            ldb.read(lbm.insidebookmetabid(),ibmb);
//            m_marketstore.m_insidemetamap
//                    [lbm.insidebookmetabid()] = ibmb;

//            InsideBookMeta ibma{};
//            ldb.read(lbm.insidebookmetaask(),ibma);
//            m_marketstore.m_insidemetamap
//                    [lbm.insidebookmetaask()] = ibma;

//            m_marketstore.m_limitid2insideidpair[lbid] =
//                make_pair(lbm.insidebookmetabid(),lbm.insidebookmetaask());

//            MerkleTree imtree{};
//            ldb.read(ibmb.orderidroot(),imtree);
//            m_marketstore.m_inside2ordertree[lbm.insidebookmetabid()] = imtree;

//            imtree.Clear();
//            ldb.read(ibma.orderidroot(),imtree);
//            m_marketstore.m_inside2ordertree[lbm.insidebookmetaask()] = imtree;

//        }
    }
}

void CreateState::loadPlayerState() {
    for ( auto team : m_teamstatemap)  {
        MerkleTree mtree;
        //s_PlayerStore.pre_init(
        this->loadMerkleMap(
                    team.second.playermetaidroot(),
                    mtree,
                    m_playerstore.m_playerstatemap);
    }

    m_playerstore.init();
}

void CreateState::loadGameState() {
    for(auto wgs : m_weekgamestatusmetamap ) {
        MerkleTree mtree{};
        this->loadMerkleMap(wgs.second.opengamestatusroot(),
                            mtree,
                            m_gamestatustore.m_gamestatsstatemap);

        mtree.clear_leaves();
        std::unordered_map<std::string,InGameProjMeta> ingameprojmap;
        this->loadMerkleMap(wgs.second.ingameprojmetaroot(),
                            mtree,
                            ingameprojmap);

        GameStatusMeta gsm;
        for(auto igp : ingameprojmap) {
            gsm.ParseFromString(ldb.read(igp.second.gamestatusmetaid()));
            m_gamestatustore.m_gamestatsstatemap[igp.second.gamestatusmetaid()] = gsm;
            m_gamestatustore.m_id2ingameprojmeta[gsm.gameinfo().id()] = igp.first;
        }

        mtree.clear_leaves();
        std::unordered_map<std::string,GameResultsMeta> gameresmap;
        this->loadMerkleMap(wgs.second.gameresultmetaroot(),
                            mtree,
                            gameresmap);

        for(auto grp : gameresmap) {
            gsm.ParseFromString(ldb.read(grp.second.gamestatusmetaid()));
            m_gamestatustore.m_gamestatsstatemap[grp.second.gamestatusmetaid()] = gsm;
        }

    }
    m_gamestatustore.init();
}

void CreateState::loadFantasyNameBalState() {
    MerkleTree mtree;
    this->loadMerkleMap(m_leaderboardmeta.fnbalmetaroot(),mtree,
                        m_fantasynamestore.m_fantasynamebalmetamap);

    m_fantasynamestore.init();
}

void CreateState::loadProjState() {
    m_projstore.m_projstatemap.insert(m_projmetamap.begin(),m_projmetamap.end());

    m_projstore.init();
}

void CreateState::process(const BlockMeta &bm) {
    TrMeta trm;
    ldb.read(bm.trmetaid(),trm);
    processTrData(trm.datametaroot());
    createTrDataState();
    processTx(bm.txmetaroot());
    createTxState();
    processTr(trm,bm.trmetaid());
    createTrState();
}

void CreateState::processTrData(const std::string &datametaroot) {
    MerkleTree datametatree;
    std::unordered_map<std::string, DataMeta> datametamap;
    loadMerkleMap(datametaroot,datametatree,datametamap);

    //dumpMerkleMap(datametamap);
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

            for (auto mid : m_gamestatustore.m_gamestatsstatemap) {
                ldb.write(mid.first,mid.second.SerializeAsString());
            }
            m_gamestatustore.dirty = true;
            break;

        case Data_Type_GAME:
            id = m_gamestatustore.process(datap.first,
                                          datap.second.data().GetExtension(GameData::game_data));
            if ( id != "")
                ldb.write(id,m_gamestatustore.m_gamestatsstatemap[id].SerializeAsString());

            break;

        case Data_Type_RESULT: {
            rd = datap.second.data().GetExtension(ResultData::result_data);
//            m_gamestatustore.m_id2ingameprojmeta[rd.game_result().gameid()]

            InGameProjMeta igpm;
            ldb.read(m_gamestatustore.m_id2ingameprojmeta[rd.game_result().gameid()],igpm);
            processGameResult(rd.game_result(),igpm,datap.first);

            break;
        }
        default:
            break;
        }
    }
}

void CreateState::processTr(const TrMeta &trmeta, const std::string &trid) {
    GlobalState gs;
    switch(trmeta.type()) {
    case TrType::WEEKOVER:
        if ( trmeta.week()+1 > 16 ) {
            gs.set_week(0);
            gs.set_season(trmeta.season()+1);
            gs.set_state(GlobalState_State_OFFSEASON);
        }
        else {
            gs.set_week(trmeta.week()+1);
            gs.set_season(trmeta.season());
        }
        m_globalstatemeta.set_prev(m_pbstate.globalstateid());
        m_globalstatemeta.set_trmetaid(trid);
        m_globalstatemeta.mutable_globalstate()->CopyFrom(gs);
        m_pbstate.set_globalstateid(ldb.write(m_globalstatemeta));
        m_projstore.clear();
        m_posstore.clear();
        m_projmetamap.clear();
        m_projmetatree.Clear();
        m_marketstore.clear();
        m_orderstore.clear();
        m_playermarketstatetree.Clear();
        m_ordermetatree.Clear();
        m_ordermetamap.clear();
        m_playermarketstatemap.clear();
        m_pbstate.set_projstateid("");
        m_pbstate.set_posstateid("");

        break;
    case TrType::GAMESTART: {
//        m_globalstatemeta.set_prev(m_pbstate.globalstateid());
//        m_globalstatemeta.set_trmetaid(trid);
//        m_globalstatemeta.mutable_globalstate()->CopyFrom(gs);
//        m_pbstate.set_globalstateid(ldb.write(m_globalstatemeta));
        MerkleTree gamemetatree;
        std::unordered_map<std::string, GameMeta> gamemetamap;
        loadMerkleMap(trmeta.gamemetaroot(),gamemetatree,gamemetamap);
        for (auto gmid : gamemetatree.leaves() )
            processGameStart(gmid,gamemetamap[gmid],trid);
        break;
    }
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

/**
 * @brief CreateState::processGameStart process GAMESTART transition
 * @param gmid gamemetaid
 * @param gmeta gamemeta
 * @param trid transitionid
 */
void CreateState::processGameStart(const string &gmid,const GameMeta &gmeta,const string &trid) {
    string id = m_gamestatustore.start(gmid,gmeta);
    if ( id != "")
        ldb.write(id,m_gamestatustore.m_gamestatsstatemap[id].SerializeAsString());

    GameStatusMeta gsm = m_gamestatustore.getGameStatusMeta(gmeta.gamedata().gameid());
    const GameInfo &gi = gsm.gameinfo();
    bool home = false;
    bool away = false;
    TeamProjMeta tpm;
    tpm.set_gameid(gmeta.gamedata().gameid());
    tpm.set_gamedatametaid(gmid);
    tpm.set_week(gsm.week());
    InGameProjMeta igpm;
    igpm.set_gameid(gmeta.gamedata().gameid());
    igpm.set_gamestatusmetaid(id);
    igpm.set_gamedatametaid(gmid);
    for ( auto ts : m_teamstatemap) {
        if ( ts.second.teamid() == gi.home() ) {
            home = true;
            tpm.set_team(gi.home());
            tpm.set_gameplayerprojmetaroot(
                        processTeamGameStart(ts.second.playermetaidroot(),gmid,id));
            igpm.set_homeprojmeta(ldb.write(tpm));
        }
        else if ( ts.second.teamid() == gi.away() ) {
            away = true;
            tpm.set_team(gi.away());
            tpm.set_gameplayerprojmetaroot(
                        processTeamGameStart(ts.second.playermetaidroot(),gmid,id));
            igpm.set_awayprojmeta(ldb.write(tpm));
        }
        if ( away && home ) {
            m_gamestatustore.addInGameProjMeta(gmeta.gamedata().gameid(),gsm.week(),ldb.write(igpm));
            break;
        }
    }
}

string CreateState::processTeamGameStart(
                                const string &pidroot,
                                const string &gdataid,
                                const string &gstatusid) {

    vector<PlayerMeta> vecp;
    this->loadMerkleMap(pidroot,vecp);

    vector<string> pids(vecp.size());
    std::transform(vecp.begin(), vecp.end(), pids.begin(),
                   [](PlayerMeta &pm) { return pm.playerid(); });

    std::sort(pids.begin(),pids.end());

    std::unordered_map<string,pair<MerkleTree,MerkleTree>> projposplayermap;
    for( auto it = m_projstore.m_projstatemap.begin(); it!=m_projstore.m_projstatemap.end();) {
        if ( find(pids.begin(),pids.end(),it->second.playerid()) == pids.end()) {
            it++;
            continue;
        }

        projposplayermap[it->second.playerid()].first.add_leaves(it->first);
        it = m_projstore.m_projstatemap.erase(it);
    }

    //std::unordered_map<string,MerkleTree> posplayermap;
    for( auto it = m_posstore.m_posstatemap.begin(); it!=m_posstore.m_posstatemap.end();) {
        if ( find(pids.begin(),pids.end(),it->second.playerid()) == pids.end()) {
            it++;
            continue;
        }

        projposplayermap[it->second.playerid()].second.add_leaves(it->first);
        it = m_posstore.m_posstatemap.erase(it);
    }


    MerkleTree gameplayerprojmetatree;
    for ( auto pidm : projposplayermap) {
        GamePlayerProjMeta gpm;
        gpm.set_playerid(pidm.first);
        gpm.set_gamedatametaid(gdataid);
        gpm.set_gamestatusmetaid(gstatusid);
        gpm.set_projmetaplayerroot(setWriteMerkle(pidm.second.first));
        gpm.set_posmetaplayerroot(setWriteMerkle(pidm.second.second));
        gameplayerprojmetatree.add_leaves(ldb.write(gpm));
    }

    m_projstore.dirty = true;
    m_posstore.dirty = true;

    return setWriteMerkle(gameplayerprojmetatree);
}

void CreateState::processGameResult(const GameResult &grslt,
                                    const InGameProjMeta &igmeta,
                                    const string &trid) {

    auto id = m_gamestatustore.close(trid,grslt.gameid());
    if ( id != "")
        ldb.write(id,m_gamestatustore.m_gamestatsstatemap[id].SerializeAsString());

    GameResultsMeta grm{};
//    optional bytes homeresultmeta = 60;
//    optional bytes awayresultmeta = 70;


    grm.set_gameid(igmeta.gameid());
    grm.set_gamestatusmetaid(id);
    grm.set_resultdatametaid(trid);

    TeamProjMeta homeprojmeta, awayprojmeta;
    TeamResultMeta homeresultsmeta, awayresultsmeta;
    ldb.read(igmeta.homeprojmeta(),homeprojmeta);
    fromProj2Results(homeprojmeta,homeresultsmeta,id, trid);
    ldb.read(igmeta.awayprojmeta(),awayprojmeta);
    fromProj2Results(awayprojmeta,awayresultsmeta,id, trid);


    qDebug() << " home ";

    for ( auto hr : grslt.home_result())
        qDebug() << hr.DebugString().data();

    homeresultsmeta.set_playerresultmetaroot(
                ProcessResults(grslt.home_result(),
                               homeprojmeta.gameplayerprojmetaroot(),
                               id,trid));


    qDebug() << " away ";

    for ( auto hr : grslt.away_result())
        qDebug() << hr.DebugString().data();

    awayresultsmeta.set_playerresultmetaroot(
                ProcessResults(grslt.away_result(),
                               awayprojmeta.gameplayerprojmetaroot(),
                               id,trid));

    grm.set_homeresultmeta(ldb.write(homeresultsmeta));
    grm.set_awayresultmeta(ldb.write(awayresultsmeta));
    m_gamestatustore.addGameResultMeta(grm.gameid(),homeprojmeta.week(),ldb.write(grm));


}
//    homeresultsmeta.set_playerresultmetaroot();
//    grslt.away_result(i).


//    optional float result = 40;
//    optional Stats stats = 50;
//    optional bytes awardmetaplayerroot = 60;
//    optional bytes pnlmetaplayerroot = 70;


//    GameStatusMeta gsm = m_gamestatustore.getGameStatusMeta(gmeta.gamedata().gameid());
//    const GameInfo &gi = gsm.gameinfo();
//    bool home = false;
//    bool away = false;
//    TeamProjMeta tpm;
//    tpm.set_gameid(gmeta.gamedata().gameid());
//    tpm.set_gamedatametaid(gmid);
//    tpm.set_week(gsm.week());
//    InGameProjMeta igpm;
//    igpm.set_gameid(gmeta.gamedata().gameid());
//    igpm.set_gamestatusmetaid(id);
//    igpm.set_gamedatametaid(gmid);
//    for ( auto ts : m_teamstatemap) {
//        if ( ts.second.teamid() == gi.home() ) {
//            home = true;
//            tpm.set_team(gi.home());
//            tpm.set_gameplayerprojmetaroot(
//                        processTeamGameStart(ts.second.playermetaidroot(),gmid,id));
//            igpm.set_homeprojmeta(ldb.write(tpm));
//        }
//        else if ( ts.second.teamid() == gi.away() ) {
//            away = true;
//            tpm.set_team(gi.away());
//            tpm.set_gameplayerprojmetaroot(
//                        processTeamGameStart(ts.second.playermetaidroot(),gmid,id));
//            igpm.set_awayprojmeta(ldb.write(tpm));
//        }
//        if ( away && home ) {
//            m_gamestatustore.addInGameProjMeta(gmeta.gamedata().gameid(),gsm.week(),ldb.write(igpm));
//            break;
//        }
//    }
//}

void CreateState::fromProj2Results(const TeamProjMeta &teamproj,TeamResultMeta &teamresult,
                                   const string &statusid, const string &trid) {

    teamresult.set_gameid(teamproj.gameid());
    teamresult.set_team(teamproj.team());
    teamresult.set_kickofftime(teamproj.kickofftime());
    teamresult.set_week(teamproj.week());
    teamresult.set_resultdatametaid(trid);
}

/**
 * @brief CreateState::ProcessResults
 * @param ingr
 * @param gameplayerprojmetaroot
 * @param id
 * @param trid
 * @return TeamResultMeta
 */
std::string CreateState::ProcessResults(
        decltype(GameResult::default_instance().home_result()) &ingr,
        const std::string &gameplayerprojmetaroot,
        const string &id,const string &trid) {

    MerkleTree playerresulttree{};
    std::unordered_map<std::string,PlayerResultMeta> pid2res;
    {
    PlayerResultMeta prm;
    prm.set_gamestatusmetaid(id);
    prm.set_resultdatametaid(trid);
    for ( auto &pr : ingr) {
        prm.set_playerid(pr.playerid());
        prm.set_result(pr.result());
        prm.mutable_stats()->CopyFrom(pr.stats());
        pid2res[pr.playerid()] = prm;
    }
    }

    MerkleTree mtree{};
    ldb.read(gameplayerprojmetaroot,mtree);
    for ( auto &gppmid : mtree.leaves()) {
        GamePlayerProjMeta gppm{};
        ldb.read(gppmid,gppm);
        auto it = pid2res.find(gppm.playerid());
        if ( it == pid2res.end())
            continue;

        PlayerResultMeta &prm = it->second;//pid2res[gppm.playerid()];
        prm.set_playerid(gppm.playerid());
        prm.set_gamestatusmetaid(id);

        { //AWARD
        MerkleTree mtree2{};
        ldb.read(gppm.projmetaplayerroot(),mtree2);
        std::unordered_map<std::string,int> nameproj;
        std::unordered_map<std::string, AwardMeta> awm{};
        for ( auto &projmid : mtree2.leaves() ) {
            ProjMeta pm{};
            ldb.read(projmid,pm);
            AwardMeta am{};
            am.set_name(pm.name());
            am.set_proj(pm.proj());
            am.set_projmetaid(projmid);
            am.set_resultdatametaid(trid);
            awm[pm.name()] = am;
            nameproj[pm.name()] = pm.proj();
        }
        DistribuePointsAvg dist(nameproj);
        auto awards = dist.distribute(prm.result(),"FantasyAgent");
        MerkleTree awardtree{};
        for ( auto &r : awards ) {
            AwardMeta &am = awm[r.first];
            am.set_award(r.second);
            awardtree.add_leaves(ldb.write(am));
            m_fantasynamestore.award(am,trid);
        }
        awardtree.set_root(makeMerkleRoot(awardtree.leaves()));
        ldb.write(awardtree);
        prm.set_awardmetaplayerroot(awardtree.root());
        }

        { //PNL
        MerkleTree mtree2{};
        ldb.read(gppm.posmetaplayerroot(),mtree2);
        std::unordered_map<std::string,pair<int,int>> namepos;
        std::unordered_map<std::string, PnlMeta> pnlm{};
        for ( auto &posmetaid : mtree2.leaves() ) {
            PosMeta pm{};
            ldb.read(posmetaid,pm);
            PnlMeta pnl{};
            pnl.set_name(pm.name());
            pnl.set_qty(pm.qty());
            pnl.set_price(pm.price());
            pnl.set_posmetaid(posmetaid);
            pnl.set_resultdatametaid(trid);
            pnlm[pm.name()] = pnl;
            namepos[pm.name()] = {pnl.qty(),pnl.price()};
        }
        SettlePositionsRawStake set(namepos);
        auto pnls = set.settle(prm.result(),"FantasyAgent");
        MerkleTree pnltree{};
        for ( auto &r : pnls ) {
            PnlMeta &pnl = pnlm[r.first];
            pnl.set_pnl(r.second);

            pnltree.add_leaves(ldb.write(pnl));
            m_fantasynamestore.pnl(pnl,trid);
        }
        pnltree.set_root(makeMerkleRoot(pnltree.leaves()));
        ldb.write(pnltree);
        prm.set_pnlmetaplayerroot(pnltree.root());
        }
        playerresulttree.add_leaves(ldb.write(prm));
        qDebug() << gppm.DebugString().data();
    }

    playerresulttree.set_root(makeMerkleRoot(playerresulttree.leaves()));
    return ldb.write(playerresulttree);
}


/**
 * @brief CreateState::processTx process transactions
 * @param txmetaid
 */
void CreateState::processTx(const std::string &txmetaid) {
    MerkleTree txtree;
    std::unordered_map<std::string, TxMeta> txtreemap;
    loadMerkleMap(txmetaid,txtree,txtreemap);

    processNameTx(txtreemap);
    processRegTx(txtreemap);
}

/**
 * @brief CreateState::processNameTx process name transactions (first)
 * @param tmap list of transaction in this block
 */
void CreateState::processNameTx(std::unordered_map<std::string, TxMeta> &tmap) {
    //dumpMerkleMap(tmap);
    std::string id;
    for ( auto nt : tmap ) {
        if ( nt.second.txtype() != TransType::NAME)
            continue;

        id = m_fantasynamestore.process_new(nt.first,nt.second.tx().GetExtension(NameTrans::name_trans));
        if ( id != "")
            ldb.write(id,m_fantasynamestore.m_fantasynamebalmetamap[id].SerializeAsString());

    }
}

/**
 * @brief CreateState::processRegTx process other (non-name) transactions
 * @param tmap list of transaction in this block
 */
void CreateState::processRegTx(std::unordered_map<std::string, TxMeta> &tmap) {
    std::string id;
    for ( auto nt : tmap ) {
        switch ( nt.second.txtype()) {
        case TransType::PROJECTION:
            id = m_projstore.process(nt.first,
                                     nt.second.tx().GetExtension(ProjectionTrans::proj_trans),
                                     nt.second.fantasy_name());

            if ( id != "")
                ldb.write(id,m_projstore.m_projstatemap[id].SerializeAsString());

            break;
        case TransType::PROJECTION_BLOCK: {
            const ProjectionTransBlock & ptb = nt.second.tx().GetExtension(ProjectionTransBlock::proj_trans_block);
            for (const PlayerPoints & pt : ptb.player_points() ) {
                id = m_projstore.process(nt.first,pt,nt.second.fantasy_name());
                if ( id != "")
                    ldb.write(id,m_projstore.m_projstatemap[id].SerializeAsString());
            }

            break;
        }
        case TransType::STAMPED: {
            auto & stamped = nt.second.tx().GetExtension(StampedTrans::stamped_trans);
            qDebug() << "new StampedTrans " << stamped.timestamp() << stamped.seqnum();
            const Transaction &t = stamped.signed_orig().trans();
            if (t.type() != TransType::EXCHANGE)
                break;

            auto emdg = t.GetExtension(ExchangeOrder::exchange_order);

            //else
            if ( emdg.type() == ExchangeOrder::NEW) {
                processNewOrder(nt.first,emdg,
                                 stamped.signed_orig().fantasy_name(),
                                 stamped.seqnum());

            }
            else if (emdg.type() == ExchangeOrder::CANCEL) {
                processCancelOrder(nt.first,emdg,
                                 stamped.signed_orig().fantasy_name(),
                                 stamped.seqnum());
            }
            //else todo: cancel

            qDebug() << "new ExchangeOrder " << emdg.DebugString().data();
//            auto name = inst.fantasy_name();
            //bool subscribe = mNameData.IsSubscribed(fn->FantasyName.alias());
            //m_marketstore.process(emdg,seqnum,fn);


            break;
        }

        default:
            break;
        }
    }
}

std::string CreateState::processNewOrder(
                        const std::string &txid,
                        const ExchangeOrder &eo,
                        const std::string &fname,
                        int32_t refnum) {

    auto ometaid = m_orderstore.process_new(txid,eo,fname,refnum);
    if ( ometaid == "" ) return "";

    OrderMeta &ometa = m_orderstore.m_ordermetamap[ometaid];
    ldb.write(ometaid,ometa.SerializeAsString());

    QString pid = ometa.playerid().data();
    auto lb = m_marketstore.m_pid2LimitBook[pid];
    if ( lb == NULL) {
        m_marketstore.addLimitbook(pid);
        lb = m_marketstore.m_pid2LimitBook[pid];
    }
    PosMeta pm{};
    m_orderstore.m_dirtypid.insert(pid.toStdString());
    if ( lb->NewOrder(ometa,pm) ) {

        while( !lb->m_qFills.isEmpty() ) {
            OrderFillMeta ofm = lb->m_qFills.dequeue();
            ofm.set_txmetaid(txid);
            auto mid = m_marketstore.newFill(ofm);
            if ( mid != "") {
                auto &mtm = m_marketstore.m_marketticmeta[mid];
                ldb.write(mid,mtm.SerializeAsString());
                ldb.write(mtm.orderfillhead(),ofm.SerializeAsString());
            }
            m_orderstore.newFill(ofm);
            auto id = m_posstore.process(ofm);
            if ( id != "")
                ldb.write(id,m_posstore.m_posstatemap[id].SerializeAsString());

        }
    }


    return "";


//    auto &tree = m_marketstore.m_pid2limitbookid[ometa.playerid()];
//    int newaggsize = ometa.size();

//    if ( tree.leaves_size() == 0) {
//        processFirstOrder(ometa,ometaid);
//        return "";
//    }


//    for ( auto l : tree.leaves() ) {
//        LimitBookMeta & lbm = m_marketstore.m_limitbookidmap[l];
//        auto &bapair = m_marketstore.m_limitid2insideidpair[l];

//        auto price = ometa.buyside() ? lbm.ask() : lbm.bid();

//        if ( ometa.buyside() && ometa.price() < lbm.ask() ||
//             !ometa.buyside() && ometa.price() > lbm.bid()) {
//            //dopassive

//            //processPassiveOrder(ometa,ometaid);
//            break;
//        }

//        else {
//            //while(true) {
//            auto ibmid = ometa.buyside() ? bapair.second : bapair.first;
//            InsideBookMeta &ibm = m_insidemetamap[ibmid];
//            int newtotal = ibm.totsize();
//            std::list<std::string> &olist = m_inside2orderlist[ibm.orderidroot()];
//            for (auto it = olist.begin(); it != olist.end();) {
//                auto iordid = *it;
//                OrderMeta &order = orderstore.m_ordermetamap[iordid];
//                int newpasssize = order.size();
//                int maxfills = std::min(newpasssize,newtotal);
//                if (  maxfills >= newpasssize ) {
//                    //DoFill(order,iordid,order.size());
//                    newtotal -= newpasssize;
//                    newaggsize -= newpasssize;
//                    newpasssize = 0;
//                }
//                else {
//                    newpasssize -= maxfills;
//                    newtotal -= maxfills;
//                    //DoFill(order,iordid,newsize);
//                    newaggsize -= maxfills;
//                }
//                int numfills = (order.size() - newpasssize);


//                if (numfills <= 0 ) {
//                    //qDebug() << "unexpected numfilles = 0 ";
//                    continue;
//                }

//                //DoFill  numfiils  order omet
//                if ( newpasssize == 0 ) olist.erase(it);
//                else it++;

//                DoFill(orderstore,order,numfills,price,iordid);

//                dirtyinsideid.push_back(ibmid);

//                if ( newtotal == 0) break;
//            }

//            lbm.set_bidsize(newtotal);
//            dirtypidlimitbook.push_back(pid);
//            DoFill(orderstore,ometa,ometa.size() - newaggsize,price,ometaid);
//        }
//    }

//    if ( newaggsize > 0 ) {
//        ometa.set_size(newaggsize);
//        insideordernew.push_back(ometaid);
//    }

//    ometa.set_prev(ometaid);

//    return update();
}

std::string CreateState::processCancelOrder(
                        const std::string &txid,
                        const ExchangeOrder &eo,
                        const std::string &fname,
                        int32_t refnum) {

    auto ometaid = m_orderstore.process_cancel(txid,eo,fname,refnum);
    if ( ometaid == "" ) return "";

    OrderMeta &ometa = m_orderstore.m_ordermetamap[ometaid];
    ldb.write(ometaid,ometa.SerializeAsString());

    QString pid = ometa.playerid().data();
    auto lb = m_marketstore.m_pid2LimitBook[pid];
    if ( lb == NULL) return "";

    PosMeta pm{};
    m_orderstore.m_dirtypid.insert(pid.toStdString());
    lb->CancelOrder(ometa);

    return "";


//    auto &tree = m_marketstore.m_pid2limitbookid[ometa.playerid()];
//    int newaggsize = ometa.size();

//    if ( tree.leaves_size() == 0) {
//        processFirstOrder(ometa,ometaid);
//        return "";
//    }


//    for ( auto l : tree.leaves() ) {
//        LimitBookMeta & lbm = m_marketstore.m_limitbookidmap[l];
//        auto &bapair = m_marketstore.m_limitid2insideidpair[l];

//        auto price = ometa.buyside() ? lbm.ask() : lbm.bid();

//        if ( ometa.buyside() && ometa.price() < lbm.ask() ||
//             !ometa.buyside() && ometa.price() > lbm.bid()) {
//            //dopassive

//            //processPassiveOrder(ometa,ometaid);
//            break;
//        }

//        else {
//            //while(true) {
//            auto ibmid = ometa.buyside() ? bapair.second : bapair.first;
//            InsideBookMeta &ibm = m_insidemetamap[ibmid];
//            int newtotal = ibm.totsize();
//            std::list<std::string> &olist = m_inside2orderlist[ibm.orderidroot()];
//            for (auto it = olist.begin(); it != olist.end();) {
//                auto iordid = *it;
//                OrderMeta &order = orderstore.m_ordermetamap[iordid];
//                int newpasssize = order.size();
//                int maxfills = std::min(newpasssize,newtotal);
//                if (  maxfills >= newpasssize ) {
//                    //DoFill(order,iordid,order.size());
//                    newtotal -= newpasssize;
//                    newaggsize -= newpasssize;
//                    newpasssize = 0;
//                }
//                else {
//                    newpasssize -= maxfills;
//                    newtotal -= maxfills;
//                    //DoFill(order,iordid,newsize);
//                    newaggsize -= maxfills;
//                }
//                int numfills = (order.size() - newpasssize);


//                if (numfills <= 0 ) {
//                    //qDebug() << "unexpected numfilles = 0 ";
//                    continue;
//                }

//                //DoFill  numfiils  order omet
//                if ( newpasssize == 0 ) olist.erase(it);
//                else it++;

//                DoFill(orderstore,order,numfills,price,iordid);

//                dirtyinsideid.push_back(ibmid);

//                if ( newtotal == 0) break;
//            }

//            lbm.set_bidsize(newtotal);
//            dirtypidlimitbook.push_back(pid);
//            DoFill(orderstore,ometa,ometa.size() - newaggsize,price,ometaid);
//        }
//    }

//    if ( newaggsize > 0 ) {
//        ometa.set_size(newaggsize);
//        insideordernew.push_back(ometaid);
//    }

//    ometa.set_prev(ometaid);

//    return update();
}

void CreateState::processFirstOrder(const OrderMeta &ordernew,
                       const std::string &newoid) {}
//    LimitBookMeta lb;
//    if ( ordernew.buyside() ) {
//        lb.set_bid(ordernew.price());
//        lb.set_bidsize(ordernew.size());
//    }
//    else {
//        lb.set_ask(ordernew.price());
//        lb.set_asksize(ordernew.size());
//    }

//    InsideBookMeta ibm;
//    ibm.set_totsize(ordernew.size());
//    MerkleTree mtree;
//    mtree.add_leaves(newoid);
//    mtree.set_root(hashit(newoid));
//    ibm.set_orderidroot(ldb.write(mtree));
//    auto ibmid = ldb.write(ibm);
//    if ( ordernew.buyside() ) {
//        lb.set_insidebookmetabid(ibmid);
//        m_marketstore.m_pidtoarbook[ordernew.playerid()].first[ordernew.price()] = ordernew.size();
//    }
//    else {
//        lb.set_insidebookmetaask(ibmid);
//        m_marketstore.m_pidtoarbook[ordernew.playerid()].second[ordernew.price()] = ordernew.size();
//    }

//    auto lbid = ldb.write(lb);

//    MerkleTree lbtree{};
//    lbtree.add_leaves(lbid);
//    lbtree.set_root(hashit(lbid));
//    auto lbroot = ldb.write(lbtree);

//    PlayerMarketState pms{};
//    auto it = m_marketstore.m_marketmetamap.find(ordernew.playerid());
//    if ( it != m_marketstore.m_marketmetamap.end()) {
//        pms.set_prev(it->first);
//        pms.set_marketticmetaid(it->second.marketticmetaid());
//    }

//    pms.set_limitbookmetaid(lbtree.root());
//    pms.set_playerid(ordernew.playerid());
//    auto pmsid = ldb.write(pms);
//    m_marketstore.m_marketmetamap[pmsid] = pms;

//    m_marketstore.m_pid2marketid[ordernew.playerid()] = pmsid;
//    m_marketstore.m_pid2limitbookid[ordernew.playerid()] = lbtree;
//    m_marketstore.m_limitbookidmap[lbid] = lb;
//    m_marketstore.m_insidemetamap[ibmid] = ibm;
//    m_marketstore.m_limitid2insideidpair.insert({lbid,{ibmid,""}});
//    m_marketstore.m_inside2ordertree[ibmid] = mtree;
//    m_marketstore.m_inside2orderlist.insert({ibmid,{newoid}});
//}

/*
void CreateState::processPassiveOrder(
                    const OrderMeta &ordernew,
                    const std::string &newoid) {

    string ibid = ordernew.buyside() ?
            m_marketstore.m_pidtoarbook[ordernew.playerid()].first[ordernew.price()] :
            m_marketstore.m_pidtoarbook[ordernew.playerid()].second[ordernew.price()];

    if ( ibid == "" ) {
        InsideBookMeta iobm;
        iobm.set_totsize(ordernew.size());
        MerkleTree tree;
        tree.add_leaves(newoid);
        tree.set_root(hashit(neworderid));
        iobm.set_orderidroot(tree.root);
        ibid = ldb.write(iobm);
        ldb.write(tree);
        m_marketstore.m_insidemetamap[ibid] = iobm;
//        if ( ordernew.buyside() )
//            m_marketstore.m_pidtoarbook[ordernew.playerid()].first[ordernew.price()] = ibid;
        (ordernew.buyside() ?
                    m_marketstore.m_pidtoarbook[ordernew.playerid()].first[ordernew.price()] :
                    m_marketstore.m_pidtoarbook[ordernew.playerid()].second[ordernew.price()] )
                  = ibid;
    }
    else {
        InsideBookMeta &iobm = m_marketstore.m_insidemetamap[ibid];
        iobm.set_totsize(iobm.totsize()+ordernew.size());
        MerkleTree tree;
        ldb.read(iobm.orderidroot(),tree);
        tree.add_leaves(newoid);
        tree.set_root(makeMerkleRoot(tree.leaves());
        ldb.write(tree);
        iobm.set_orderidroot(tree.root);
        ibid = ldb.write(iobm);
        m_marketstore.m_insidemetamap[ibid] = iobm;
        (ordernew.buyside() ?
                    m_marketstore.m_pidtoarbook[ordernew.playerid()].first[ordernew.price()] :
                    m_marketstore.m_pidtoarbook[ordernew.playerid()].second[ordernew.price()] )
                  = ibid;
    }



    MerkleTree &tree =  m_marketstore.m_pid2limitbookid[ordernew.playerid()];

//    bool workingask = false;
//    bool workingbid = false;

//    LimitBookMeta lbm2;
//    std::vector<LimitBookMeta> newlimit;// = newlimitsmap[ordernew.playerid()];

    for ( int i = 0; i < tree.leaves_size(); i++) {
        LimitBookMeta &lbm = m_limitbookidmap[tree.leaves(i)];
        if ( isa ) {
            if ( pr > lbm.ask() ) continue;

            if ( pr < lbm.ask() ) {
                lb.set_ask(ordernew.price());
                lb.set_asksize(ordernew.size());
            }
        }
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
            else if ( ordernew.buyside() && lbm.bidsize() == 0 ) {
                if (lbm.asksize() != 0) {
                    lbm2.set_ask(lbm.ask());
                    lbm2.set_asksize(lbm.asksize());
                    workingask = true;
                    continue;
                }

            }
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

    LimitBookMeta lb;
    if ( ordernew.buyside() ) {
        lb.set_bid(ordernew.price());
        lb.set_bidsize(ordernew.size());
    }
    else {
        lb.set_ask(ordernew.price());
        lb.set_asksize(ordernew.size());
    }

    InsideBookMeta ibm;
    ibm.set_totsize(ordernew.size());
    MerkleTree mtree;
    mtree.add_leaves(newoid);
    mtree.set_root(hashit(newoid));
    ibm.set_orderidroot(ldb.write(mtree));
    auto ibmid = ldb.write(ibm);
    if ( ordernew.buyside() )
        lb.set_insidebookmetabid(ibmid);
    else
        lb.set_insidebookmetaask(ibmid);

    auto lbid = ldb.write(lb);

    MerkleTree lbtree{};
    lbtree.add_leaves(lbid);
    lbtree.set_root(hashit(lbid));
    auto lbroot = ldb.write(lbtree);

    PlayerMarketState pms{};
    auto it = m_marketstore.m_marketmetamap.find(ordernew.playerid());
    if ( it != m_marketstore.m_marketmetamap.end()) {
        pms.set_prev(it->first);
        pms.set_marketticmetaid(it->second.marketticmetaid());
    }

    pms.set_limitbookmetaid(lbtree.root());
    pms.set_playerid(ordernew.playerid());
    auto pmsid = ldb.write(pms);
    m_marketstore.m_marketmetamap[pmsid] = pms;

    m_marketstore.m_pid2marketid[ordernew.playerid()] = pmsid;
    m_marketstore.m_pid2limitbookid[ordernew.playerid()] = lbtree;
    m_marketstore.m_limitbookidmap[lbid] = lb;
    m_marketstore.m_insidemetamap[ibmid] = ibm;
    m_marketstore.m_limitid2insideidpair.insert({lbid,{ibmid,""}});
    m_marketstore.m_inside2ordertree[ibmid] = mtree;
    m_marketstore.m_inside2orderlist.insert({ibmid,{newoid}});
    */
//}


void CreateState::createTrDataState() {
    createTrPlayerDataState();
    createTrGameDataState();
    createTrLeaderboardState();
}

void CreateState::createTrPlayerDataState() {
    auto tmr = m_playerstore.createPlayermetaidroots();
    if ( tmr.size() == 0 ) return;

    for(int i = 0; i < m_teamstatetree.leaves_size(); i++) {
        std::string id = m_teamstatetree.leaves(i);

        auto teamid = m_teamstatemap[id].teamid();
        if ( tmr.find(teamid) != tmr.end() ) {
            TeamMeta &tm = m_teamstatemap[id];
            auto &pmerkle = tmr[teamid];
            tm.set_playermetaidroot(pmerkle.root());
            tm.set_prev(id);
            ldb.write(pmerkle.root(),pmerkle.SerializeAsString());
            auto ts = tm.SerializeAsString();
            auto th = hashit(ts);
            m_teamstatetree.set_leaves(i,th);
            ldb.write(th,ts);
        }
    }

    m_teamstatetree.set_root(makeMerkleRoot(m_teamstatetree.leaves()));
    ldb.write(m_teamstatetree.root(),m_teamstatetree.SerializeAsString());
    this->loadMerkleMap(m_teamstatetree.root(),
                        m_teamstatetree,
                        m_teamstatemap);
    m_playerstore.clean();
    m_pbstate.set_teamstatemid(m_teamstatetree.root());
}

/**
 * @brief CreateState::createTrGameDataState set the new Game status meta state
 *          this is done after processing GameData and updating GameStatusStore
 *          m_pbstate.set_schedulestateid(m_weekgamestatusmetatree.root());

 */
void CreateState::createTrGameDataState() {
    auto tmr = m_gamestatustore.createGameStatusmetaidroots();
    if ( tmr.size() == 0 ) return;

    if ( m_gamestatustore.dirty == true) {
        m_weekgamestatusmetatree.clear_leaves();
        m_weekgamestatusmetamap.clear();
        for ( auto &trees : tmr) {
            WeekGameStatusMeta wgsm;
            wgsm.set_week(trees.first);
            ldb.write(trees.second[0].root(),trees.second[0].SerializeAsString());
            wgsm.set_opengamestatusroot(trees.second[0].root());
            ldb.write(trees.second[1].root(),trees.second[1].SerializeAsString());
            wgsm.set_ingameprojmetaroot(trees.second[1].root());
            ldb.write(trees.second[2].root(),trees.second[2].SerializeAsString());
            wgsm.set_gameresultmetaroot(trees.second[2].root());
            auto id = ldb.write(wgsm);
            m_weekgamestatusmetamap[id] = wgsm;
            m_weekgamestatusmetatree.add_leaves(id);
        }

        m_weekgamestatusmetatree.set_root(makeMerkleRoot(m_weekgamestatusmetatree.leaves()));
        ldb.write(m_weekgamestatusmetatree.root(),m_weekgamestatusmetatree.SerializeAsString());
        m_pbstate.set_schedulestateid(m_weekgamestatusmetatree.root());
        m_gamestatustore.clean();
    }
    else {
        for(int i = 0; i < m_weekgamestatusmetatree.leaves_size(); i++) {
            std::string id = m_weekgamestatusmetatree.leaves(i);

            int week = m_weekgamestatusmetamap[id].week();
            if ( tmr.find(week) != tmr.end() ) {
                WeekGameStatusMeta tm = m_weekgamestatusmetamap[id];
                tm.set_opengamestatusroot(tmr[week][0].root());
                ldb.write(tmr[week][0].root(),tmr[week][0].SerializeAsString());
                tm.set_ingameprojmetaroot(tmr[week][1].root());
                ldb.write(tmr[week][1].root(),tmr[week][1].SerializeAsString());
                tm.set_gameresultmetaroot(tmr[week][2].root());
                ldb.write(tmr[week][2].root(),tmr[week][2].SerializeAsString());
                tm.set_prev(id);
                auto ts = tm.SerializeAsString();
                auto th = hashit(ts);
                m_weekgamestatusmetatree.set_leaves(i,th);
                m_weekgamestatusmetamap.erase(id);
                m_weekgamestatusmetamap.insert({th,tm});
                ldb.write(th,ts);
                m_gamestatustore.dirtyweek[week] = false;
            }
        }

        m_weekgamestatusmetatree.set_root(makeMerkleRoot(m_weekgamestatusmetatree.leaves()));
        ldb.write(m_weekgamestatusmetatree.root(),m_weekgamestatusmetatree.SerializeAsString());
        this->loadMerkleMap(m_weekgamestatusmetatree.root(),
                            m_weekgamestatusmetatree,
                            m_weekgamestatusmetamap);
        m_gamestatustore.clean();
        m_pbstate.set_schedulestateid(m_weekgamestatusmetatree.root());
    }
}

void CreateState::createTrLeaderboardState() {
    for (auto newam : m_fantasynamestore.m_pendingnew ) {
        auto &fnb = m_fantasynamestore.m_pendingnew[newam.first];

        auto it = m_fantasynamestore.m_pendingawards.find(newam.first);
        if ( it != m_fantasynamestore.m_pendingawards.end()) {
            auto &mtree = m_fantasynamestore.m_pendingawards[newam.first];
            mtree.set_root(makeMerkleRoot(mtree.leaves()));
            fnb.set_awardmetaroot(ldb.write(mtree));
        }
        auto it2 = m_fantasynamestore.m_pendingpnl.find(newam.first);
        if ( it2 != m_fantasynamestore.m_pendingpnl.end()) {
            auto &mtree = m_fantasynamestore.m_pendingpnl[newam.first];
            mtree.set_root(makeMerkleRoot(mtree.leaves()));
            fnb.set_pnlmetaroot(ldb.write(mtree));
        }

        auto id = m_fantasynamestore.update(fnb);
        if ( id != "")
            ldb.write(id,m_fantasynamestore.m_fantasynamebalmetamap[id].SerializeAsString());

    }
    m_fantasynamestore.m_pendingnew.clear();
    m_fantasynamestore.m_pendingawards.clear();
    m_fantasynamestore.m_pendingpnl.clear();

    //createNameTxState();

}

void CreateState::createTrState() {
    createTrGameDataState();
    createProjState();
    createTrLeaderboardState();
}

/**
 * @brief CreateState::createTxState create new states after processing transactions
 */
void CreateState::createTxState() {
    createNameTxState();
    createProjState();
    createMarketOrderState();
    createPosState();
}

void CreateState::createMarketOrderState() {

    for ( auto ref : m_orderstore.m_dirtyorders ) {
        auto &oldid = m_orderstore.m_refnum2orderid[ref];
        OrderMeta om = m_orderstore.m_ordermetamap[oldid];
        om.set_prev(oldid);
        m_orderstore.m_ordermetamap.erase(oldid);
        auto it = m_orderstore.m_orderFills.find(ref);
        if ( it != m_orderstore.m_orderFills.end()) {
            MerkleTree &mt = it->second;
            mt.set_root(makeMerkleRoot(mt.leaves()));
            om.set_orderfillmetaid(ldb.write(mt));
        }

        auto newoid = ldb.write(om);
        m_orderstore.m_ordermetamap[newoid] = om;
        m_orderstore.m_refnum2orderid[ref] = newoid;
    }

    m_orderstore.m_dirtyorders.clear();

    for ( auto pid : m_orderstore.m_dirtypid) {
        LimitBook &lb = *m_marketstore.m_pid2LimitBook[pid.data()];
        int bprice = lb.getBestBid();
        int aprice = lb.getBestAsk();
        PlayerMarketState pms{};
        auto it = m_marketstore.m_pid2marketid.find(pid);
        if ( it != m_marketstore.m_pid2marketid.end()) {
            pms.set_prev(it->first);
            m_marketstore.m_marketmetamap.erase(it->second);
        }
        pms.set_playerid(pid);
        MerkleTree limitbook{};
        while(bprice > 0 || aprice <= 40) {
            LimitBookMeta lbm;
            while ( bprice > 0 ) {
                auto ib = lb.getInside(true,bprice);
                if ( ib->totSize <= 0) {
                    bprice--;
                    continue;
                }
                lbm.set_bid(bprice);
                lbm.set_bidsize(ib->totSize);
                MerkleTree mtree;
                for (auto iiter = ib->top(); iiter != ib->bot(); iiter++) {
                    //todo orderfillmetaid
                    mtree.add_leaves(m_orderstore.m_refnum2orderid[iiter->refnum()]);
                }
                mtree.set_root(makeMerkleRoot(mtree.leaves()));
                lbm.set_bidordermetaroot(ldb.write(mtree));
                bprice--;
                break;
            }
            while ( aprice <= 40 ) {
                auto ib = lb.getInside(false,aprice);
                if ( ib->totSize <= 0) {
                    aprice++;
                    continue;
                }
                lbm.set_ask(aprice);
                lbm.set_asksize(ib->totSize);
                MerkleTree mtree;
                for (auto iiter = ib->top(); iiter != ib->bot(); iiter++) {
                    //todo orderfillmetaid
                    mtree.add_leaves(m_orderstore.m_refnum2orderid[iiter->refnum()]);
                }
                mtree.set_root(makeMerkleRoot(mtree.leaves()));
                lbm.set_askordermetaroot(ldb.write(mtree));
                aprice++;
                break;
            }
            limitbook.add_leaves(ldb.write(lbm));
        }
        limitbook.set_root(makeMerkleRoot(limitbook.leaves()));
        {
        auto it = m_marketstore.m_pid2marketticid.find(pid);
        if ( it != m_marketstore.m_pid2marketticid.end())
            pms.set_marketticmetaid(it->second);
        }
        pms.set_limitbookmetaid(ldb.write(limitbook));
        m_marketstore.m_marketmetamap[ldb.write(pms)] = pms;
    }

    MerkleTree mtree;
    for ( auto it : m_marketstore.m_marketmetamap) {
        mtree.add_leaves(it.first);
    }
    mtree.set_root(makeMerkleRoot(mtree.leaves()));
    m_pbstate.set_marketstateid(ldb.write(mtree));


//    if (m_orderstore.m_dirtypid.size() > 0)
//    for (m_playermarketstatemap
    m_orderstore.m_dirtypid.clear();


/*
    for ( auto oref : m_orderstore.m_neworders) {
        OrderMeta &order = m_orderstore.m_ordermetamap[m_orderstore.m_refnum2orderid[oref]];

        //m_marketstore.process(m_orderstore,oref);
    }

    for ( auto fl : m_orderstore.m_newfills) {
        auto &filldata = m_orderstore.m_orderfillmetamap[fl];

        auto id = m_posstore.process(filldata);
        if ( id != "")
            ldb.write(id,m_posstore.m_posstatemap[id].SerializeAsString());
    }
*/
}

/**
 * @brief CreateState::createNameTxState create leaderboard state and store it in
 *     m_pbstate.set_leaderboardstateid(ldb.write(m_leaderboardmeta));

 */
void CreateState::createNameTxState() {
    if ( m_fantasynamestore.dirty.size() == 0) return;

    MerkleTree mtree;
    for (auto nmid : m_fantasynamestore.m_name2metaid) {
        mtree.add_leaves(nmid.second);
    }

//    auto tmr = m_playerstore.createPlayermetaidroots();

    mtree.set_root(makeMerkleRoot(mtree.leaves()));
    ldb.write(mtree.root(),mtree.SerializeAsString());
    m_leaderboardmeta.set_prev(m_pbstate.leaderboardstateid());
    m_leaderboardmeta.set_fnbalmetaroot(mtree.root());
    m_pbstate.set_leaderboardstateid(ldb.write(m_leaderboardmeta));
    m_fantasynamestore.dirty.clear();

}

/**
 * @brief CreateState::createProjState create proj state store it
    m_pbstate.set_projstateid(m_projmetatree.root());
 */
void CreateState::createProjState() {
//    if ( m_projstore.dirtyplayerfname.size() > 0 && !m_projstore.dirty) return;

    if (m_projstore.dirty) {
        m_projstore.init();
        m_projmetamap.clear();
        m_projmetatree.clear_leaves();
        m_projmetamap = m_projstore.m_projstatemap;
        setNewMerkelTree(m_projmetamap,m_projmetatree);
        m_pbstate.set_projstateid(m_projmetatree.root());
    }
    else if ( m_projstore.dirtyplayerfname.size() > 0 ||
                        m_projstore.newprojmeta.size() > 0) {

        if ( m_projstore.newprojmeta.size()  > 0) {
            for ( auto np : m_projstore.newprojmeta) {
                auto newstateid = m_projstore.m_projid2metaid[np];
                m_projmetatree.add_leaves(newstateid);
                m_projmetamap.insert(*m_projstore.m_projstatemap.find(newstateid));
                m_projstore.dirtyplayerfname.erase(np);
            }

            m_projstore.newprojmeta.clear();
        }

        if ( m_projstore.dirtyplayerfname.size() > 0 ) {
            for ( int i =0; i < m_projmetatree.leaves_size(); i++) {
                const ProjMeta &pm = m_projmetamap[m_projmetatree.leaves(i)];
                std::string id = ProjStore::makeid(pm.playerid(),pm.name());
                if ( m_projstore.dirtyplayerfname.find(id) !=
                                 m_projstore.dirtyplayerfname.end()) {
                    auto &eid = m_projmetatree.leaves(i);
                    m_projmetamap.erase(eid);
                    m_projstore.m_projstatemap.erase(eid);
                    auto newstateid = m_projstore.m_projid2metaid[id];
                    m_projmetatree.set_leaves(i,newstateid);
                    m_projmetamap.insert(*m_projstore.m_projstatemap.find(newstateid));
                    //m_projstore.dirtyplayerfname[id] = false;
                }
            }

            m_projstore.dirtyplayerfname.clear();
        }


        m_projmetatree.set_root(makeMerkleRoot(m_projmetatree.leaves()));
        ldb.write(m_projmetatree.root(),m_projmetatree.SerializeAsString());
        m_pbstate.set_projstateid(m_projmetatree.root());
    }


//        m_projmetamap.clear();
//        this->loadMerkleMap(m_pbstate.projstateid(),
//                            m_projmetatree,
//                            m_projmetamap);

}

/**
 * @brief CreateState::createPosState create pos state
 */
void CreateState::createPosState() {
    if (m_posstore.dirty) {
        m_posstore.init();
        m_posmetamap.clear();
        m_posmetatree.clear_leaves();
        m_posmetamap = m_posstore.m_posstatemap;
        setNewMerkelTree(m_posmetamap,m_posmetatree);
        m_pbstate.set_posstateid(m_posmetatree.root());
    }
    else if ( m_posstore.dirtyplayerfname.size() > 0 ||
          m_posstore.newposmeta.size() > 0) {

        if ( m_posstore.newposmeta.size()  > 0) {
            for ( auto np : m_posstore.newposmeta) {
                auto newstateid = m_posstore.m_posid2metaid[np];
                m_posmetatree.add_leaves(newstateid);
                m_posmetamap.insert(*m_posstore.m_posstatemap.find(newstateid));
                m_posstore.dirtyplayerfname.erase(np);
            }

            m_posstore.newposmeta.clear();
        }

        if ( m_posstore.dirtyplayerfname.size() > 0 ) {
            for ( int i =0; i < m_posmetatree.leaves_size(); i++) {
                const PosMeta &pm = m_posmetamap[m_posmetatree.leaves(i)];
                std::string id = PosStore::makeid(pm.playerid(),pm.name());
                if ( m_posstore.dirtyplayerfname.find(id) !=
                     m_posstore.dirtyplayerfname.end()) {
                    auto &eid = m_posmetatree.leaves(i);
                    m_posmetamap.erase(eid);
                    m_posstore.m_posstatemap.erase(eid);
                    auto newstateid = m_posstore.m_posid2metaid[id];
                    m_posmetatree.set_leaves(i,newstateid);
                    m_posmetamap.insert(*m_posstore.m_posstatemap.find(newstateid));
                    //m_projstore.dirtyplayerfname[id] = false;
                }
            }

            m_posstore.dirtyplayerfname.clear();
        }

        m_posmetatree.set_root(makeMerkleRoot(m_posmetatree.leaves()));
        ldb.write(m_posmetatree.root(),m_posmetatree.SerializeAsString());
        m_pbstate.set_posstateid(m_posmetatree.root());
    }
}

decltype(CreateState::GENESIS_NFL_TEAMS) CreateState::GENESIS_NFL_TEAMS {
    "ARI" ,
    "ATL" ,
    "BAL" ,
    "BUF" ,
    "CAR" ,
    "CHI" ,
    "CIN" ,
    "CLE" ,
    "DAL" ,
    "DEN" ,
    "DET" ,
    "GB" ,
    "HOU" ,
    "IND" ,
    "JAC" ,
    "KC" ,
    "MIA" ,
    "MIN" ,
    "NE" ,
    "NO" ,
    "NYG" ,
    "NYJ" ,
    "OAK" ,
    "PHI" ,
    "PIT" ,
    "SD" ,
    "SEA" ,
    "SF" ,
    "STL" ,
    "TB" ,
    "TEN" ,
    "WAS",
    "FA"
};
}
