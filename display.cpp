#include "display.h"

#include <QDebug>

using namespace std;

Display::Display(QObject *parent) : QObject(parent)
{
    bc.SeekToFirst();
    QString qstr = QString::fromStdString(bc.GetCurrentBlock().signedhead().DebugString());
    this->setBlockHeader(qstr);

    this->setTransactions(this->getTransactionsString());
}

void Display::prevPressedSlot() {
    bc.Prev();
    QString qstr = QString::fromStdString(bc.GetCurrentBlock().signedhead().DebugString());
    this->setBlockHeader(qstr);

    this->setTransactions(this->getTransactionsString());
}

void Display::nextPressedSlot() {
    bc.Next();
    QString qstr = QString::fromStdString(bc.GetCurrentBlock().signedhead().DebugString());
    this->setBlockHeader(qstr);

    this->setTransactions(this->getTransactionsString());
}

QString Display::blockHeader() const {
    return this->m_block;
}

void Display::setBlockHeader(QString txt) {
    this->m_block = txt;
    emit blockHeaderChanged();
}

QString Display::transactions() const {
    return this->m_transactions;
}

void Display::setTransactions(QString txt) {
    this->m_transactions = txt;
    emit transactionsChanged();

}

QString Display::getTransactionsString() {
    string trans = "";

    for (int i = 0; i < bc.GetCurrentBlock().signed_transactions().size(); ++i) {
        fantasybit::SignedTransaction tran = bc.GetCurrentBlock().signed_transactions(i);
        trans += tran.DebugString() + '\n';
    }

    return QString::fromStdString(trans);
}
