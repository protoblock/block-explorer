#include "datastores.h"
#include "createstate.h"

namespace fantasybit_bx {

using namespace std;

void PlayerStore::init() {
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

std::string PlayerStore::process(const std::string &dataid, const PlayerData &pd) {
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

std::string PlayerStore::update(const PlayerMeta &pm, const std::string &prevteam) {
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

std::unordered_map<std::string, MerkleTree> PlayerStore::createPlayermetaidroots() {
    std::unordered_map<std::string,std::string> ret;
    std::unordered_map<std::string,MerkleTree> mtrees;
    for ( auto d : dirtyeteam ) {
        if ( d.second )
            mtrees[d.first] = MerkleTree::default_instance();
    }
    for ( auto pt : m_playerid2teamid) {
        if ( dirtyeteam[pt.second] )
            mtrees[pt.second].add_leaves(m_playerid2metaid[pt.first]);
    }
    for ( auto &tmt : mtrees) {
        tmt.second.set_root(makeMerkleRoot(tmt.second.leaves()));
    }

    return mtrees;
}



void PlayerStore::clean() {
    for( auto d : CreateState::GENESIS_NFL_TEAMS) {
        dirtyeteam[d] = false;
    }
}

void FantasyNameStore::init() {
    for ( auto fnb : m_fantasynamebalmetamap) {
        m_name2metaid[fnb.second.name()] = fnb.first;
    }
}

std::string FantasyNameStore::process_new(const std::string &txid, const NameTrans &nt) {
    //FantasyNameBal fnb;
    FantasyNameBalMeta fbm;
    fbm.set_name(nt.fantasy_name());
    fbm.set_public_key(nt.public_key());
    fbm.set_bits(0);
    fbm.set_stake(0);
    fbm.set_txmetaid(txid);

    return update(fbm);
}

std::string FantasyNameStore::update(const FantasyNameBalMeta &gm) {
    auto newid = hashit(gm.SerializeAsString());
    m_fantasynamebalmetamap[newid] = gm;
    m_name2metaid[gm.name()] = newid;
    dirty.push_back(gm.name());

    return newid;
}

std::string FantasyNameStore::award(const AwardMeta &am, const string &trid) {
    if ( am.name() == "fantasy trader")
        qDebug() << "";

    FantasyNameBalMeta *fnbm;
    MerkleTree *ammt;
    auto it = m_pendingnew.find(am.name());
    if ( it == m_pendingnew.end()) {
        m_pendingnew[am.name()] = FantasyNameBalMeta::default_instance();
        fnbm = &m_pendingnew[am.name()];
        auto it2 = m_name2metaid.find(am.name());
        if ( it2 != m_name2metaid.end()) {
            fnbm->CopyFrom(m_fantasynamebalmetamap[it2->second]);
            fnbm->set_prev(it2->second);
            fnbm->mutable_awardmetaroot()->clear();
            fnbm->mutable_pnlmetaroot()->clear();
            fnbm->mutable_trmetaid()->clear();
            fnbm->mutable_txmetaid()->clear();
        }
        m_pendingawards[am.name()] = MerkleTree::default_instance();

        ammt = &m_pendingawards[am.name()];
        fnbm->set_trmetaid(trid);
    }
    else {
        fnbm = &m_pendingnew[am.name()];
        if ( m_pendingawards.find(am.name()) == m_pendingawards.end())
            m_pendingawards[am.name()] = MerkleTree::default_instance();

        ammt = &m_pendingawards[am.name()];
    }


    ammt->add_leaves(hashit(am));
    fnbm->set_bits(am.award() + fnbm->bits());
    fnbm->set_stake(am.award() + fnbm->stake());

//    qDebug() << "adding award" << am.award() << " :: " << fnbm->name().data() <<
//             fnbm->public_key().data() << fnbm->bits();


    return "";
}

std::string FantasyNameStore::pnl(const PnlMeta &am, const string &trid) {
    FantasyNameBalMeta *fnbm;
    MerkleTree *ammt;
    auto it = m_pendingnew.find(am.name());
    if ( it == m_pendingnew.end()) {
        m_pendingnew[am.name()] = FantasyNameBalMeta::default_instance();
        fnbm = &m_pendingnew[am.name()];
        auto it2 = m_name2metaid.find(am.name());
        if ( it2 != m_name2metaid.end()) {
            fnbm->CopyFrom(m_fantasynamebalmetamap[it2->second]);
            fnbm->set_prev(it2->second);
            fnbm->mutable_awardmetaroot()->clear();
            fnbm->mutable_pnlmetaroot()->clear();
            fnbm->mutable_trmetaid()->clear();
            fnbm->mutable_txmetaid()->clear();
        }
        m_pendingpnl[am.name()] = MerkleTree::default_instance();

        ammt = &m_pendingpnl[am.name()];
        fnbm->set_trmetaid(trid);
    }
    else {
        fnbm = &m_pendingnew[am.name()];
        if ( m_pendingpnl.find(am.name()) == m_pendingpnl.end())
            m_pendingpnl[am.name()] = MerkleTree::default_instance();

        ammt = &m_pendingpnl[am.name()];
    }

    ammt->add_leaves(hashit(am));
    fnbm->set_stake(fnbm->stake() + am.pnl());

    qDebug() << "adding pnl" << am.pnl() << " :: " << fnbm->name().data() <<
             fnbm->public_key().data() << fnbm->bits();

    return "";
}


void GameStatusStore::init() {
    dirty = false;
    for ( auto gmp : m_gamestatsstatemap) {
        m_gameid2metaid[gmp.second.id()] = gmp.first;
        m_gameid2week[gmp.second.id()] = gmp.second.week();
        if ( dirtyweek.find(gmp.second.week()) == dirtyweek.end())
            dirtyweek[gmp.second.week()] = false;

    }
}

std::string GameStatusStore::process(const std::string &dataid, const ScheduleData &sd) {
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

std::string GameStatusStore::process(const std::string &dataid, const GameData &gd) {
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

std::string GameStatusStore::process(const std::string &dataid, const ResultData &gd) {
    return "";
}

std::string GameStatusStore::update(const GameStatusMeta &gm) {
    auto newid = hashit(gm.SerializeAsString());
    m_gamestatsstatemap[newid] = gm;
    m_gameid2metaid[gm.id()] = newid;

    dirtyweek[gm.week()] = true;

    return newid;
}

std::string GameStatusStore::close(const std::string &dataid, const std::string &id) {
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

std::string GameStatusStore::start(const std::string &gmid, const GameMeta &gm) {
    GameStatusMeta gsm;
    auto it = m_gameid2metaid.find(gm.gamedata().gameid());
    if ( it == m_gameid2metaid.end())
        return "";

    gsm.CopyFrom(m_gamestatsstatemap[it->second]);
    gsm.set_prev(it->second);
    gsm.mutable_gamesatus()->set_status(GameStatus::INGAME);
    gsm.set_gamemetaid(gmid);

    return update(gsm);
}

std::map<int, std::vector<MerkleTree> > GameStatusStore::createGameStatusmetaidroots() {
    std::map<int,std::vector<MerkleTree>> ret;
    std::unordered_map<int,MerkleTree> mtrees;
    std::unordered_map<int,MerkleTree> mtrees_ingame;

    for ( auto d : dirtyweek ) {
        if ( d.second ) {
            mtrees[d.first] = MerkleTree::default_instance();
            mtrees_ingame[d.first] = MerkleTree::default_instance();
            ret[d.first] = std::vector<MerkleTree>(3);
            //ret[d.first].set_week(d.first);
        }
    }

    for ( auto pt : m_gameid2week) {
        if ( dirtyweek[pt.second] ) {
            std::string &id = m_gameid2metaid[pt.first];
            GameStatusMeta &gsm = m_gamestatsstatemap[id];
            //ToDo:: other status
            if ( gsm.gamesatus().status() == GameStatus::SCHEDULED ) {
                ret[pt.second][0].add_leaves(id);
            }
            if ( gsm.gamesatus().status() == GameStatus::INGAME ) {
                std::string &iid = m_id2ingameprojmeta[pt.first];
                ret[pt.second][1].add_leaves(iid);
            }
            if ( gsm.gamesatus().status() == GameStatus::CLOSED ) {
                std::string &iiid = m_id2gameresultmeta[pt.first];
                ret[pt.second][2].add_leaves(iiid);
            }

        }
    }


    for ( auto &tmt : ret) {
        for ( auto &v : tmt.second)
            v.set_root(makeMerkleRoot(v.leaves()));
    }

//    for ( auto tmt : mtrees_ingame) {
//        ret[tmt.first].set_ingameprojmetaroot(makeMerkleRoot(tmt.second.leaves()));
//    }

    return ret;
}

void GameStatusStore::clean() {
    dirty = false;
    for( auto d : dirtyweek) {
        dirtyweek[d.first] = false;
    }
}

void ProjStore::init() {
    for ( auto pmp : m_projstatemap) {
        auto id = makeid(pmp.second.playerid(),pmp.second.name());
        m_projid2metaid[id] = pmp.first;
        //dirtyplayerfname[id] = false;
    }
    dirtyplayerfname.clear();
    dirty = false;
}

std::string ProjStore::process(const std::string &txid, const ProjectionTrans &pj, const std::string &fname) {
    ProjMeta pm;
    auto pf = makeid(pj.playerid(),fname);
    auto it = m_projid2metaid.find(pf);
    if ( it != m_projid2metaid.end()) {
        pm.set_prev(it->second);
        if ( newprojmeta.find(pf) != newprojmeta.end() )
            m_projstatemap.erase(it->second);
        else if ( dirtyplayerfname.find(pf) !=  dirtyplayerfname.end())
            m_projstatemap.erase(it->second);
        else
            dirtyplayerfname[pf] = true;
    }
    else
        newprojmeta.insert(pf);

    pm.set_name(fname);
    pm.set_playerid(pj.playerid());
    pm.set_proj(pj.points());
    pm.set_txmetaid(txid);
    return update(pm,pf);
}

std::string ProjStore::process(const std::string &txid, const PlayerPoints &pj, const std::string &fname) {
    ProjMeta pm;
    if ( pj.playerid() == "1088" && fname == "Show me More of your TDs")
        qDebug() << " 1088 " << pj.DebugString().data();

    auto pf = makeid(pj.playerid(),fname);
    auto it = m_projid2metaid.find(pf);
    if ( it != m_projid2metaid.end()) {
        pm.set_prev(it->second);       
        if ( newprojmeta.find(pf) != newprojmeta.end() )
            m_projstatemap.erase(it->second);
        else if ( dirtyplayerfname.find(pf) !=  dirtyplayerfname.end())
            m_projstatemap.erase(it->second);
        else
            dirtyplayerfname[pf] = true;
    }
    else
        newprojmeta.insert(pf);

    pm.set_name(fname);
    pm.set_playerid(pj.playerid());
    pm.set_proj(pj.points());
    pm.set_txmetaid(txid);
    return update(pm,pf);
}

std::string ProjStore::update(const ProjMeta &pm, const std::string &pf) {
    auto newid = hashit(pm.SerializeAsString());
    m_projstatemap[newid] = pm;
    m_projid2metaid[pf] = newid;

    return newid;
}

void GameStatusStore::addInGameProjMeta(const std::string &gameid, int week, const std::string &ingameprojid) {
    m_id2ingameprojmeta[gameid] = ingameprojid;
    dirtyweek[week] = true;
}

void GameStatusStore::addGameResultMeta(const std::string &gameid, int week, const std::string &resultid) {
    m_id2gameresultmeta[gameid] = resultid;
    dirtyweek[week] = true;
}



}

