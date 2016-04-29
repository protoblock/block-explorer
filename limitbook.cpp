#include "limitbook.h"
#include <QDebug>
using namespace std;

namespace fantasybit_bx {

using namespace fantasybit;
LimitBook::LimitBook(const QString &playerid, QObject *parent)
    : QObject(parent), m_playerId(playerid) {
    m_bestBid = -1;
    m_bestAsk = BOOK_SIZE;
}

int LimitBook::getBestAsk() const
{
    return m_bestAsk+1;
}

int LimitBook::getBestBid() const
{
    return m_bestBid+1;
}

void LimitBook::GetTop(bool isbuy) {
    if (isbuy) {
        for (; m_bestBid >= 0 && m_bids[m_bestBid].totSize == 0; --m_bestBid) ;
        if (m_bestBid < 0)
            NewTop(0,0,isbuy);
        else
            NewTop(m_bestBid+1, m_bids[m_bestBid].totSize, isbuy);
    }
    else {
        for (; m_bestAsk < BOOK_SIZE && m_asks[m_bestAsk].totSize == 0; ++m_bestAsk) ;
        if (m_bestAsk >= BOOK_SIZE)
            NewTop(0,0,isbuy);
        else
            NewTop(m_bestAsk+1, m_asks[m_bestAsk].totSize, isbuy);
    }
}

//void LimitBook::SaveRemove(Order &o,int32_t fillqty) {

//#ifdef TRACE
//        qDebug() << "level2 SaveRemove " << o.DebugString() << "fillqty" << fillqty;
//#endif

//    auto no = mBookDelta->add_removes();
//    no->CopyFrom(o);
//    no->mutable_core()->set_size(fillqty);

//#ifdef TRACE
//        qDebug() << "level2 SaveRemove muted" << no->DebugString() << "fillqty" << fillqty;
//#endif

//}

#ifdef FALSE
void LimitBook::SaveNew(const OrderCore &oc) {
#ifdef TRACE
    string s = oc.buyside() ? "Bid" : "Ask";
    qDebug() << "level2 New SaveNew " << s  << oc.price() << oc.size();
#endif

    mBookDelta->mutable_newnew()->CopyFrom(oc);
}
#endif

//void LimitBook::NewTop(int price, int32_t qty, bool isbuy) {
//    MarketTicker *pmt = MarketTicker::default_instance().New();
//    MarketTicker &mt = *pmt;
//    mt.set_type(isbuy ? MarketTicker::BID : MarketTicker::ASK);
//    mt.set_price(price);
//    mt.set_size(qty);
//    mBookDelta->add_level1tic()->CopyFrom(mt);
//    //emit pExchangeData->get()->NewMarketTicker(pmt);
//    pExchangeData->get()->OnMarketTicker(mPlayerid,pmt);


//#ifdef TRACE
//    string s = isbuy ? "Bid" : "Ask";
//    qDebug() << "level2 NewTop " <<  s << price << qty;
//#endif

//}

void LimitBook::NewDepth(bool isbuy,int price) {
#if FALSE
#ifdef TRACE
        qDebug() << "level2 NewDepth " << isbuy << "price" << price;
#endif

    DepthFeedDelta *df = DepthFeedDelta::default_instance().New();

    df->set_isbid(isbuy);
    df->set_price(price+1);
    df->set_size(isbuy ? mBids[price].totSize : mAsks[price].totSize);


#ifdef TRACE
        qDebug() << "level2 NewDepth " << df->DebugString();
#endif
    pExchangeData->get()->OnNewDepth(mPlayerid,df);
#endif

}

bool LimitBook::NewOrder(Order &o, Position &posdelta) {
    bool haspos = false;
    //mBookDelta->Clear();
//    mBookDelta->set_seqnum(o.refnum());
    //mBookDelta->set_allocated_newnew(o.mutable_core());
    if ( o.buyside() ) {
        if ( o.price() > 0 && o.size() > 0 && o.size() <= 100000 )
            haspos = NewBid(o,posdelta);
        else
            qWarning() << "level2 invalid order";
    }
    else {
        if ( o.price() <= 40 && o.size() > 0 && o.size() <= 100000 )
            haspos = NewAsk(o,posdelta);
        else
            qWarning() << "level2 invalid order";
    }

    return haspos;
}

int32_t LimitBook::CancelOrder(Order &order) {
    //mBookDelta->Clear();
    auto myprice = order.price()-1;
    if ( (myprice < 0) || (myprice >= BOOK_SIZE) )
        return -1;

    if (order.buyside()) {
        m_bids[myprice].Cancel(order);
        //SaveRemove(order,order.core().size());
        //mBookDelta->add_removes()->CopyFrom(order);
        //{
        //Send(new BookFeedData(ExecType.Canceled, order));

        if (m_bestBid == myprice) {
            if (m_bids[myprice].totSize > 0)
                NewTop(myprice+1, m_bids[myprice].totSize, true);
            else
                GetTop(true);
        }

//        NewDepth(true, myprice);
        //} else
        //Send(new BookFeedData(ExecType.CancelReject, order));
    }
    else {
        m_asks[myprice].Cancel(order);
        //Send(new BookFeedData(ExecType.Canceled, order));
//        SaveRemove(order,order.size());
        if (m_bestAsk == myprice) {
            if (m_asks[myprice].totSize > 0)
                NewTop(myprice+1, m_asks[myprice].totSize, false);
            else
                GetTop(false);
        }

//        NewDepth(false, myprice);
        //} else
        //    //mFeed.Execution(order, BookFeed.OrdStatus.CancelReject, 0);
        //    Send(new BookFeedData(ExecType.CancelReject, order));
    }
}

bool LimitBook::NewBid(Order &order, Position &deltapos) {
#ifdef TRACE
qDebug() << "level2 New Bid " << order.price() << order.size();
#endif


    auto myprice = order.price()-1;
    if ( (myprice < 0) || (myprice >= BOOK_SIZE) )
        return false;

    bool havepos = false;

    if (myprice < m_bestAsk) {
        //if (order.constraint == Constraint.IOC)
        //    Send(new BookFeedData(ExecType.Canceled, order));
        // else
        //{
         m_bids[myprice].New(order);
#ifdef TRACE
    qDebug() << "level2 NewBid 1 mBb" <<  m_bestBid;
#endif

         if (myprice >= m_bestBid) {
             m_bestBid = myprice;
             NewTop(myprice+1, m_bids[myprice].totSize, true);
         }
         NewNew(order);
     }
    else {
         int fillqty = order.size();
#ifdef TRACE
        qDebug() << " order.size() " << order.size() << " before";
#endif

         int32_t pos =
                 SweepAsks(order); //will modify size
#ifdef TRACE
        qDebug() << " order.size() " << order.size() << " after";
#endif

         GetTop(false);
         fillqty = fillqty - order.size();

         deltapos.set_price(-pos);
         deltapos.set_qty(fillqty);
         havepos = true;

         if (order.size() <= 0)
             ;//Send(new BookFeedData(ExecType.Done, order));
         else {
             //if (order.constraint == Constraint.IOC)
             //    Send(new BookFeedData(ExecType.Canceled, order));
             //else {
#ifdef TRACE
    qDebug() << "level2 NewBid 6 m_bestBid" <<  m_bestBid;
#endif

             m_bids[myprice].New(order);
             m_bestBid = myprice;
#ifdef TRACE
    qDebug() << "level2 NewBid 7 m_bestBid" <<  m_bestBid;
#endif

             NewTop(myprice+1, m_bids[myprice].totSize, true);
             NewNew(order);


             //mFeed.Executi    on(order, BookFeed.OrdStatus.New, 0);
             //Send(new BookFeedData(ExecType.New,order));
             //}
         }
     }

    return havepos;
}

bool LimitBook::NewAsk(Order &order, Position &deltapos) {
#ifdef TRACE
    qDebug() << order.refnum() << "level2 New Ask " << order.price() << order.size();
#endif

    auto myprice = order.price()-1;
    if ( (myprice < 0) || (myprice >= BOOK_SIZE) )
       return false;

    bool havepos = false;

    if (myprice > m_bestBid) {
       //if (order.constraint == Constraint.IOC)
       //    Send(new BookFeedData(ExecType.Canceled, order));
       // else
       //{
        m_asks[myprice].New(order);
        if (myprice <= m_bestAsk) {
            m_bestAsk = myprice;
            NewTop(myprice+1, m_asks[myprice].totSize, false);
        }
        NewNew(order);
        //}
    }
    else {

#ifdef TRACE
        qDebug() << order.refnum() << " order.size() " << order.size() << " before";
#endif
        int fillqty = order.size();
        int32_t pos =
                SweepBids(order); //will modify size
#ifdef TRACE
        qDebug() << order.refnum() << " order.size() " << order.size() << " after";
#endif

        GetTop(true);
        fillqty = fillqty - order.size();

        deltapos.set_price(pos);
        deltapos.set_qty(-fillqty);
        havepos = true;

        if (order.size() <= 0)
            ;//Send(new BookFeedData(ExecType.Done, order));
        else {
            //if (order.constraint == Constraint.IOC)
            //    Send(new BookFeedData(ExecType.Canceled, order));
            //else {
            m_asks[myprice].New(order);
            m_bestAsk = myprice;
            NewTop(myprice+1, m_asks[myprice].totSize, false);
            NewNew(order);

            //mFeed.Execution(order, BookFeed.OrdStatus.New, 0);
            //Send(new BookFeedData(ExecType.New,order));
            //}
        }
    }

    return havepos;
}

int32_t LimitBook::SweepAsks( Order &order) {
    int price = order.price()-1;
    int left = order.size();
    int32_t pos = 0;
    for (; m_bestAsk <= price; ++m_bestAsk) {
        InsideBook &curr = m_asks[m_bestAsk];
#ifdef TRACE
qDebug() << order.refnum() << "level2 SweepAsks  curr " << m_bestBid << curr.totSize << " ord core " << order.size();
#endif

        int fillqty = min(curr.totSize, left);
        if (fillqty <= 0)
            continue;
#ifdef TRACE
qDebug() << order.refnum() << "level2 SweepAsks  fillqty continue " << fillqty;
#endif

        pos += (m_bestAsk+1) * fillqty; //for instafill

        SendFill(order, fillqty, m_bestAsk, false);
        //left -= fillqty;
        //NewTrade(m_bestAsk, fillqty, Side.BID);

        for (auto iiter = curr.top();
             iiter != curr.bot() && left > 0;) {
            Order &ord = *iiter;
#ifdef TRACE
qDebug() << order.refnum() << "level2 SweepAsks  curr ord iityer" << m_bestBid << ord.DebugString().data() << " left " << left;
#endif

            fillqty = min(ord.size(), left);
            if (fillqty <= 0)
                continue;

            left -= fillqty;

            SendFill(ord, fillqty, m_bestAsk, true);
            //ord.mutable_core()->set_size(ord.core().size()-fillqty);
//            SaveRemove(ord,fillqty);
            if (curr.Fill(fillqty, iiter))
                ;//Send(new BookFeedData(ExecType.Done, ord));
        }

#ifdef TRACE
qDebug() << order.refnum() << "level2 SweepAsks  curr bottom" << m_bestBid << curr.totSize;
#endif
        NewDepth(false, m_bestAsk);

        if (left <= 0)
            break;
    }

    return pos;
}

int32_t LimitBook::SweepBids( Order &order) {
    int price = order.price()-1;
    int left = order.size();
#ifdef TRACE
qDebug() << order.refnum() << "level2 sweepbids  top ord core" << order.size();
#endif
    int32_t pos = 0;
    for (; m_bestBid >= price; --m_bestBid) {
        InsideBook &curr = m_bids[m_bestBid];

#ifdef TRACE
qDebug() << order.refnum() << "level2 sweepbids  curr " << m_bestBid << curr.totSize << " ord core " << order.size();
#endif

        int fillqty = min(curr.totSize, left);
        if (fillqty <= 0)
            continue;

#ifdef TRACE
qDebug() << order.refnum() << "level2 sweepbids  fillqty continue " << fillqty;
#endif

        pos += (m_bestBid+1) * fillqty; //for instafill

        SendFill(order, fillqty, m_bestBid, false);
        //NewTrade(m_bestBid, fillqty, Side.ASK);

        for (auto iiter = curr.top();
             iiter != curr.bot() && left > 0;) {
            Order &ord = *iiter;
#ifdef TRACE
qDebug() << order.refnum() << "level2 sweepbids  curr ord iityer" << m_bestBid << ord.DebugString().data() << " left " << left;
#endif

            fillqty = min(ord.size(), left);
            if (fillqty <= 0)
                continue;

            left -= fillqty;

            SendFill(ord, fillqty, m_bestBid, true);
            //ord.mutable_core()->set_size(ord.core().size()-fillqty);
//            SaveRemove(ord,fillqty);
            if (curr.Fill(fillqty, iiter))
                ;//Send(new BookFeedData(ExecType.Done, ord));
        }

#ifdef TRACE
qDebug() << order.refnum() << "level2 sweepbids  curr bottom" << m_bestBid << curr.totSize;
#endif

        NewDepth(true, m_bestBid);

        if (left <= 0)
            break;
    }

    return pos;
}


void LimitBook::SendFill(Order &o, int32_t q, int price, bool ispassive ) {

#ifdef TRACE
        qDebug() << o.refnum() << "level2 SendFill " << q << price << ispassive << o.DebugString().data();
#endif

    price += 1;
    o.set_size(o.size()-q);

    //if ( ispassive )
    {
        int32_t deltaqty;
        int32_t deltapos;
        if (o.buyside() ) {
            deltaqty = q;
            deltapos = -q * price;
        }
        else {
            deltaqty = -q;
            deltapos = q * price;
        }

        OrderFillMeta ofm;
        ofm.set_refnum(o.refnum());
        ofm.set_fname(o.fname());
        ofm.set_playerid(o.playerid());
        ofm.set_fillprice(price);
        ofm.set_fillsize(q);
        ofm.set_buyside(o.buyside());

        m_qFills.enqueue(ofm);

        emit orderFill(o.refnum());
//        pExchangeData->get()->
//                OnDeltaPos(mPlayerid, o.refnum(),deltaqty, deltapos);
    }
    /*
    auto it = pExchangeData->get()->mSeqNameMap.find(o.refnum());
    if ( it == end(pExchangeData->get()->mSeqNameMap))
        qCritical() << "level2 cant find order for fill" << o.DebugString();
    else {

        auto &mypos = mPkPos[it->second];
        auto size = q * (o.buyside() ? 1 : -1);
        mypos.netqty += size;
        mypos.netprice += size * (price+1) * -1;
    //fo.refnum(),q,price+1
        pExchangeData->get()->OnOrderPositionChange(o,mypos,this->mPlayerid,it->second);
    }
    */

#if FALSE
    if ( ispassive )
        return;

    MarketTicker *lst = mBookDelta->add_level1tic();
    lst->set_type(MarketTicker::LAST);
    lst->set_price(price);
    lst->set_size(q);

    TradeTic *tt = TradeTic::default_instance().New();
    tt->set_price(price);
    tt->set_size(q);

    uint64_t timestamp = (std::chrono::duration_cast<std::chrono::milliseconds>
                      (std::chrono::system_clock::now().time_since_epoch()).count());

    auto dt = currentNewYorkTime();
    pExchangeData->get()->OnTrade(mPlayerid,tt);
#ifdef TIMEAGENTWRITEFILLS
#ifndef TIMEAGENTWRITEFILLS_FORCE
    if ( !pExchangeData->get()->amlive )
        return;

#else
#endif

    qDebug() << " sql epoch " << timestamp;

    SqlStuff sql("satoshifantasy","ticker");
    sql.fill(mPlayerid,tt,pExchangeData->get()->mWeek,timestamp,o.refnum());

#endif
#endif
}

void LimitBook::NewNew(Order &order) {
    //pExchangeData->get()->OnNewOrder(order);
    emit newNew(order.refnum());

    //SaveNew(order.core());
    NewDepth(order.buyside(), order.price()-1);

#ifdef TRACE
        qDebug() << order.refnum() << "level2 NewNew " << order.DebugString().data();
#endif

}

void LimitBook::NewTop(int price, int32_t qty, bool isbuy) {
//    MarketTicker *pmt = MarketTicker::default_instance().New();
//    MarketTicker &mt = *pmt;
//    mt.set_type(isbuy ? MarketTicker::BID : MarketTicker::ASK);
//    mt.set_price(price);
//    mt.set_size(qty);
//    mBookDelta->add_level1tic()->CopyFrom(mt);
//    //emit pExchangeData->get()->NewMarketTicker(pmt);
//    pExchangeData->get()->OnMarketTicker(mPlayerid,pmt);
    emit newTop(m_playerId);

#ifdef TRACE
    string s = isbuy ? "Bid" : "Ask";
    qDebug() << "level2 NewTop " <<  s.data() << price << qty;
#endif

}
}
