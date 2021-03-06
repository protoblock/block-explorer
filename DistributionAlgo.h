//
//  DistributionAlgo.h
//  fantasybit
//
//  Created by Jay Berg on 3/19/14.
//
//

#ifndef __fantasybit__DistributionAlgo__
#define __fantasybit__DistributionAlgo__

//#include "fbutils.h"
//#include "generated/ExData.pb.h"
#include <iostream>
#include <unordered_map>

namespace fantasybit_bx
{

template<class T>
using NameValuePairs =
    std::unordered_map < std::string, T > ;

    
class DistributionAlgo {};

class DistributePoints : public DistributionAlgo
{
public:
    DistributePoints(const NameValuePairs<int> &p) : projections(p) {}
    
protected:
    NameValuePairs<int> projections;
};

struct DistribuePointsAvg : DistributePoints
{
    DistribuePointsAvg(const NameValuePairs<int> &p) : DistributePoints(p) {}
    NameValuePairs<int> distribute(const double result, const std::string &agent) const;
};


class SettlementAlgo {};

class SettlePositions : public SettlementAlgo
{
public:
    SettlePositions(const NameValuePairs<std::pair<int,int>> &p) : positions(p) {}

protected:
    NameValuePairs<std::pair<int,int>> positions;
};

using PnlResults = NameValuePairs<int>;

struct SettlePositionsRawStake : SettlePositions {
    SettlePositionsRawStake(const NameValuePairs<std::pair<int,int>> &p) : SettlePositions(p) {}
    PnlResults settle(const double result, const std::string &agent) const;
};


}

#endif /* defined(__fantasybit__DistributionAlgo__) */
