#include "createstate.h"


namespace fantasybit {

std::string CreateState::createState(const BlockMeta &bm) {

    if ( getStateId(bm.prev()) != m_pbstateid)
        m_pbstateid = loadStateId(bm.prev());

    if ( bm.pbstateid() != m_pbstateid ) {
        return "error"; //ToDo:
    }


    process(bm);
    pbstate pbs{};

    auto pbstatestr =  pbs.SerializeAsString();
    auto pbstateid = fc::sha256::hash(pbstatestr).str();

    return pbstateid;
}

std::string fantasybit::CreateState::loadStateId(const std::string &blockmetaid) {
    std::string pbstatemetaid = getStateId(blockmetaid);

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

    return hashit(pbstatemetastr);
}

std::string fantasybit::CreateState::getStateId(const std::string &blockmetaid) {
    std::string ret = "";

    auto blockmetastr = ldb.read(blockmetaid);
    BlockMeta bm;
    if ( bm.ParseFromString(blockmetastr) )
        ret = bm.pbstateid();

    return ret;
}

}
