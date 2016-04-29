#include "insidebook.h"

namespace fantasybit_bx {

InsideBook::InsideBook(QObject *parent)
    : QObject(parent), totSize(0) {}

std::list<OrderMeta>::iterator InsideBook::top() {
    return begin(mOList);
}

std::list<OrderMeta>::iterator InsideBook::bot() {
    return end(mOList);
}

void InsideBook::New(OrderMeta &order) {
    totSize += order.size();
    mOMap.insert(make_pair(order.refnum(),mOList.insert(mOList.end(),order)));
    return;
}

void InsideBook::Cancel(fantasybit_bx::OrderMeta &order) {
    auto it = mOMap.find(order.refnum());
    if ( it == end(mOMap) )
        return;

    if ( it->second == end(mOList))
        return;


    auto ret = it->second->size();
    totSize -= ret;

    mOList.erase(it->second);
    mOMap.erase(it);

    //orderIdCount++;
    //return ret;
    return;
}

bool InsideBook::Fill(int32_t qty, std::list<OrderMeta>::iterator &iter) {
    totSize -= qty;

    /*
        auto newsize = iter->core().size() - qty;
        if ( newsize > 0 ) {
            iter->mutable_core()->set_size(newsize);
        }
        */
    if ( iter->size() == 0) {
        mOMap.erase(iter->refnum());
        iter = mOList.erase(iter);
    }

    return true;
}

bool InsideBook::Fill(int32_t qty, std::list<OrderMeta>::reverse_iterator &iter) {
#ifdef TRACE
//    qDebug() << "level2 qty " << qty << iter->core().size() << totSize;
#endif
    totSize -= qty;
    /*
        auto newsize = iter->core().size() - qty;
        if ( newsize > 0 ) {
            iter->mutable_core()->set_size(newsize);
        }
        */
    if (iter->size() == 0) {
        mOMap.erase(iter->refnum());

        std::list<OrderMeta>::iterator tempIter = mOList.erase( --iter.base());
        iter = std::list<OrderMeta>::reverse_iterator(tempIter);
    }

    return true;
}

void InsideBook::Remove(fantasybit_bx::OrderMeta &order) {
    auto it = mOMap.find(order.refnum());
    if ( it == end(mOMap) )
        return;

    if ( it->second == end(mOList))
        return;

    auto sz = order.size();
    totSize -= sz;

    auto newsz = it->second->size() - sz;
    if ( newsz > 0 )
        it->second->set_size(newsz);
    else {
        mOList.erase(it->second);
        mOMap.erase(it);
    }
    //orderIdCount++;
    //return ret;
    //return orderIdCount;
}


}

