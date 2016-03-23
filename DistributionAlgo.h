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

namespace fantasybit
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


//class SettlementAlgo {};

//class SettlePositions : public SettlementAlgo
//{
//public:
//    SettlePositions(const BookPos &p) : positions(p) {}

//protected:
//   const BookPos &positions;
//};

//using PnlResults = NameValuePairs<std::pair<SettlePos,Int> >;

//struct SettlePositionsRawStake : SettlePositions {
//    SettlePositionsRawStake(const BookPos &p) : SettlePositions(p) {}
//    PnlResults settle(const double result, const std::string &agent) const;
//};


}

#endif /* defined(__fantasybit__DistributionAlgo__) */
