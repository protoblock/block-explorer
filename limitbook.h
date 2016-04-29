#ifndef LIMITBOOK_H
#define LIMITBOOK_H

#include <QObject>
#include "insidebook.h"
#include "qstring.h"
#include <QQueue>


namespace fantasybit_bx {
#define BOOK_SIZE 40

class LimitBook : public QObject {
    Q_OBJECT
public:
    explicit LimitBook(const QString &playerid, QObject *parent = 0);

private:
    QString m_playerId;

    using Order = fantasybit::OrderMeta;
    using Position = fantasybit::PosMeta;

    InsideBook m_bids[BOOK_SIZE], m_asks[BOOK_SIZE];

    int m_bestBid, m_bestAsk;

public:
    bool NewBid(Order &order, Position &);
    bool NewAsk(Order &order, Position &);
    void NewTop(int price, int32_t qty, bool isbuy);
    void NewNew(Order &order);

    void NewDepth(bool isbuy,int price);
    int32_t SweepAsks( Order &order);
    int32_t SweepBids( Order &order);
    void GetTop(bool isbuy);

    void SaveRemove(Order &o,int32_t fillqty);
    //void SaveNew(const OrderCore &oc);
    void SendFill(Order &o, int32_t q, int price, bool );
    bool Locked = false;
public:
    QQueue<fantasybit::OrderFillMeta> m_qFills{} ;

    InsideBook *getInside(bool bid, int32_t price) {
        return bid ? &m_bids[price-1] : &m_asks[price-1];
    }

    void updateTopfromCache(int b, int a) {
        if ( b > 0 && b <= BOOK_SIZE)
            m_bestBid = b-1;
        if ( a > 0 && a <= BOOK_SIZE)
            m_bestAsk = a-1;
    }

    bool NewOrder(Order &eo, Position &);

    int32_t CancelOrder(Order &order);
    int getBestBid() const;

    int getBestAsk() const;

signals:
    void orderFill(qint32);
    void newNew(qint32);
    void newTop(QString&);

public slots:
};

}

#endif // LIMITBOOK_H
