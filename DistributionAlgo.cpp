//
//  DistributionAlgo.cpp
//  fantasybit
//
//  Created by Jay Berg on 3/19/14.
//
//

#include "DistributionAlgo.h"
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <assert.h>
//#include "core.h"
#include <QDebug>

namespace fantasybit_bx {

using namespace std;

NameValuePairs<int>
    DistribuePointsAvg::distribute(const double result, const std::string &agent) const
{
    NameValuePairs<int> award{};
    if (projections.size() == 0 || result <= 0.0001 ) {
        qInfo() << "no projections agent " << agent.data() << " gets balance " << result;
        if ( result > 0.0001 )
            award[agent] = result * 100.0;
        return award;
	}
	
    double mean = 0;
    vector<double> diffs;
    diffs.reserve(projections.size());
    
    for(const auto& pair : projections) {
        double diff = result - pair.second;
        diff = fabs(diff);
        mean+=diff;
        diffs.emplace_back(diff);

//        qInfo() << pair.first.data() << " projection " << pair.second << " diff " << diff;
    }
  
    mean /= projections.size();
    //qInfo() << "mean " << mean;

    
    double maxdiff = min(mean, result);
    //qInfo() << " maxdiff " << maxdiff;


    double sum = 0.0;
    for ( double d : diffs) {
        if ( d <= maxdiff ) {
            sum = sum + (result - d);
        }
    }

    if (sum == 0.0) {
        qInfo() << "no projections within 100% " << agent.data() << " gets balance " << result;
        award[agent] = result * 100.0;
        return award;
    }

    /*
    
    double sum2 = accumulate(begin(diffs), end(diffs), 0,
        [maxdiff,result](const double insum,const double val)
        {
            return insum + ((val <= maxdiff) ? (result-val) : 0.0);
        });

    //qInfo() << " sum " << sum << " sum2" << sum2;
    */

    double payout = result / sum;

//    qInfo() << " sum " << sum << " payout " << payout;

	double total = 0.0;
    for (const auto& pair : projections) {
        double diff = fabs(result - pair.second);
		if (diff <= maxdiff)
		{
			double amount = (result - diff)*payout;
            award[pair.first] = amount * 100.0;
			total += amount;
//            qDebug() << pair.first.data() << " projection " << pair.second << " award " << amount;
		}
        else;
//            qDebug() << pair.first.data() << " projection " << pair.second << "no award ";
    }

    if (result < total) {
        qCritical() << "gave out to much" << result << total;
    }
    else {
        double leftover = result - total;
        if (leftover > 0.00001) {
            int hold = award[agent];
            award[agent] = hold + leftover;
            qDebug() << "agent " << agent.data() << " leftovers " << leftover;

        }
    }
    return award;
}

//#include "ExchangeData.h"

PnlResults SettlePositionsRawStake::
            settle(
                const double result,
                const std::string &agent
            ) const {

    PnlResults pnl{};
    if (positions.size() == 0)
        return pnl;

    //double mean = 0;
    //vector<double> diffs;
    //diffs.reserve(positions.size());
    int intresult = floor((result * 100.0) + 0.5);

    for(const auto& settlepos : positions) {
        int hispnl = (settlepos.second.first * intresult) + settlepos.second.second * 100;
        pnl[settlepos.first] = hispnl;
        qDebug() << settlepos.first.data() <<
                    " pnl " <<  hispnl << " result " << result <<
                    " pos " << settlepos.second.first << " " << settlepos.second.second;
    }

    return pnl;
}

}
