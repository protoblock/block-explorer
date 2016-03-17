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

    std::unordered_map<std::string,std::string> createPlayermetaidroots();

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

    std::unordered_map<int, WeekGameStatusMeta>
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

    bool dirty;
};

}

#endif // DATASTORES_H
