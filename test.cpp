#include "test.h"

#include <QDebug>

using namespace std;

Test::Test(QObject *parent) : QObject(parent)
{
    bc.SeekToFirst();
    QString qstr = QString::fromStdString(bc.GetCurrentBlock().signedhead().DebugString());
    this->setBlockHeader(qstr);

    this->setTransactions(this->getTransactionsString());
}

void Test::prevPressedSlot() {
    bc.Prev();
    QString qstr = QString::fromStdString(bc.GetCurrentBlock().signedhead().DebugString());
    this->setBlockHeader(qstr);

    this->setTransactions(this->getTransactionsString());
}

void Test::nextPressedSlot() {
    bc.Next();
    QString qstr = QString::fromStdString(bc.GetCurrentBlock().signedhead().DebugString());
    this->setBlockHeader(qstr);

    this->setTransactions(this->getTransactionsString());
}

QString Test::blockHeader() const {
    return this->m_block;
}

void Test::setBlockHeader(QString txt) {
    this->m_block = txt;
    emit blockHeaderChanged();
}

QString Test::transactions() const {
    return this->m_transactions;
}

void Test::setTransactions(QString txt) {
    this->m_transactions = txt;
    emit transactionsChanged();

}

QString Test::getTransactionsString() {
    string trans = "";

    for (int i = 0; i < bc.GetCurrentBlock().signed_transactions().size(); ++i) {
        fantasybit::SignedTransaction tran = bc.GetCurrentBlock().signed_transactions(i);
        trans += tran.DebugString() + '\n';
    }

    return QString::fromStdString(trans);
}
