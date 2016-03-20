#include "createstate.h"
#include "datastores.h"

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
        auto htm = hashit(tm);
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
        MerkleTree mtree;
        this->loadMerkleMap(wgs.second.opengamestatusroot(),
                            mtree,
                            m_gamestatustore.m_gamestatsstatemap);

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

        break;
    case TrType::GAMESTART: {
        m_globalstatemeta.set_prev(m_pbstate.globalstateid());
        m_globalstatemeta.set_trmetaid(trid);
        m_globalstatemeta.mutable_globalstate()->CopyFrom(gs);
        m_pbstate.set_globalstateid(ldb.write(m_globalstatemeta));
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




    //get home and away team
    //get players
    //remove proj
    //set game proj
    //clear orders
    //set settlepos
    //update WeekGameStatusMeta
    //  ingameprojmetaroot
    //  opengamestatusroot
    //auto teamid = m_teamstatemap[id].teamid();

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

    std::unordered_map<string,MerkleTree> projplayermap;
    for( auto it = m_projstore.m_projstatemap.begin(); it!=m_projstore.m_projstatemap.end();) {
        if ( find(pids.begin(),pids.end(),it->second.playerid()) == pids.end()) {
            it++;
            continue;
        }

        projplayermap[it->second.playerid()].add_leaves(it->first);
        it = m_projstore.m_projstatemap.erase(it);
    }

    MerkleTree gameplayerprojmetatree;
    for ( auto pidm : projplayermap) {
        GamePlayerProjMeta gpm;
        gpm.set_playerid(pidm.first);
        gpm.set_gamedatametaid(gdataid);
        gpm.set_gamestatusmetaid(gstatusid);
        gpm.set_posmetaplayerroot(setWriteMerkle(pidm.second));
        gameplayerprojmetatree.add_leaves(ldb.write(gpm));
    }

    m_projstore.dirty = true;

    return setWriteMerkle(gameplayerprojmetatree);
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
        default:
            break;
        }
    }
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
            wgsm.set_ingameprojmetaroot(trees.second[2].root());
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
                WeekGameStatusMeta &tm = m_weekgamestatusmetamap[id];
                tm.set_opengamestatusroot(tmr[week][0].root());
                tm.set_ingameprojmetaroot(tmr[week][1].root());
                tm.set_gameresultmetaroot(tmr[week][2].root());
                tm.set_prev(id);
                auto ts = tm.SerializeAsString();
                auto th = hashit(ts);
                m_weekgamestatusmetatree.set_leaves(i,th);
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

void CreateState::createTrState() {
    createTrGameDataState();
}

/**
 * @brief CreateState::createTxState create new states after processing transactions
 */
void CreateState::createTxState() {
    createNameTxState();
    createProjState();
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
                    m_projmetamap.erase(m_projmetatree.leaves(i));
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
