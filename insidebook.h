#ifndef INSIDEBOOK_H
#define INSIDEBOOK_H

#include <QObject>
#include "proto/StateData.pb.h"
#include "utils.h"
#include <unordered_map>

namespace fantasybit_bx {
/**
 * @brief The InsideBook struct represents a single price in a
 *      limitbook for a trading matching engine
 *      it has a totSize, which is mosly used as the BidSize or AskSize
 *          this size, also knows as qty, is the sum of all orders for
 *          this price and side
 *              100 bid @ 10
 *              qty side price
 *
 *      InsideBook manages the qty, 100 in this case.
 *
 *          these 100 qty are the result of many trade orders that were either
 *          to high or to low for execution. Instead they sit in a
 *      "limit order book"  - see @LimitBook
 */

using fantasybit::OrderMeta;
class InsideBook : public QObject
{
    Q_OBJECT
public:
    explicit InsideBook(QObject *parent = 0);

    qint32 totSize;
    std::list<OrderMeta> mOList;
    std::unordered_map<int32_t,std::list<OrderMeta>::iterator> mOMap;
    //static int32_t orderIdCount;

    std::list<OrderMeta>::iterator top();

    std::list<OrderMeta>::iterator bot();

    std::list<OrderMeta>::reverse_iterator rtop();

    std::list<OrderMeta>::reverse_iterator rbot();

    void New(OrderMeta& order);

    void Cancel(OrderMeta &order);

    bool Fill(int32_t qty,std::list<OrderMeta>::iterator &iter);

    bool Fill(int32_t qty,std::list<OrderMeta>::reverse_iterator &iter);

    void Remove(OrderMeta &order);
signals:

public slots:
};
}


#endif // INSIDEBOOK_H
