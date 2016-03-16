#include "createstate.h"
#include "datastores.h"

namespace fantasybit {

void CreateState::dump(const std::string &pbstateid) {
    //m_pbstateid = loadStateId(pbstateid);

    qDebug() << "test" << m_pbstate.DebugString().data();
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

    dump("");
    process(bm);
    dump("");

    auto pbstatestr =  m_pbstate.SerializeAsString();
    auto pbstateid = fc::sha256::hash(pbstatestr).str();

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

void CreateState::createTrPlayerDataState() {
    auto tmr = m_playerstore.createPlayermetaidroots();
    if ( tmr.size() == 0 ) return;

    for(int i = 0; i < m_teamstatetree.leaves_size(); i++) {
        std::string id = m_teamstatetree.leaves(i);

        auto teamid = m_teamstatemap[id].teamid();
        if ( tmr.find(teamid) != tmr.end() ) {
            TeamMeta &tm = m_teamstatemap[id];
            tm.set_playermetaidroot(tmr[teamid]);
            tm.set_prev(id);
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

void CreateState::processTx(const std::string &txmetaid) {
    MerkleTree txtree;
    std::unordered_map<std::string, TxMeta> txtreemap;
    loadMerkleMap(txmetaid,txtree,txtreemap);

    processNameTx(txtreemap);
    processRegTx(txtreemap);
}

void CreateState::processNameTx(std::unordered_map<std::string, TxMeta> &tmap) {
    dumpMerkleMap(tmap);
    std::string id;
    for ( auto nt : tmap ) {
        if ( nt.second.txtype() != TransType::NAME)
            continue;

        id = m_fantasynamestore.process_new(nt.first,nt.second.tx().GetExtension(NameTrans::name_trans));
        if ( id != "")
            ldb.write(id,m_fantasynamestore.m_fantasynamebalmetamap[id].SerializeAsString());

    }
}

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

void CreateState::createTxState() {
    createNameTxState();
    createProjState();

}

void CreateState::createNameTxState() {
    if ( m_fantasynamestore.dirty.size() == 0) return;

    MerkleTree mtree;
    for (auto nmid : m_fantasynamestore.m_name2metaid) {
        mtree.add_leaves(nmid.second);
    }

    auto tmr = m_playerstore.createPlayermetaidroots();

    mtree.set_root(makeMerkleRoot(mtree.leaves()));
    ldb.write(mtree.root(),mtree.SerializeAsString());
    m_leaderboardmeta.set_prev(m_pbstate.leaderboardstateid());
    m_leaderboardmeta.set_fnbalmetaroot(mtree.root());
    m_pbstate.set_leaderboardstateid(ldb.write(m_leaderboardmeta));
    m_fantasynamestore.dirty.clear();

}

void CreateState::createProjState() {
    if ( m_projstore.dirtyplayerfname.size() == 0) return;

    //m_projmetatree.clear_leaves();
    for ( int i =0; i < m_projmetatree.leaves_size(); i++) {
        const ProjMeta &pm = m_projmetamap[m_projmetatree.leaves(i)];
        std::string id = ProjStore::makeid(pm.playerid(),pm.name());
        if ( m_projstore.dirtyplayerfname[id]) {
            m_projmetatree.set_leaves(i,m_projstore.m_projid2metaid[id]);
            m_projstore.dirtyplayerfname[id] = false;
        }
    }

    for ( auto np : m_projstore.newprojmeta) {
        m_projmetatree.add_leaves(m_projstore.m_projid2metaid[np]);
    }

    m_projmetatree.set_root(makeMerkleRoot(m_projmetatree.leaves()));
    ldb.write(m_projmetatree.root(),m_projmetatree.SerializeAsString());
    m_pbstate.set_projstateid(m_projmetatree.root());
    m_projstore.newprojmeta.clear();
    m_projmetamap.clear();
    this->loadMerkleMap(m_pbstate.projstateid(),
                        m_projmetatree,
                        m_projmetamap);

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
