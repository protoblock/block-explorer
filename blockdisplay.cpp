#include "blockdisplay.h"


BlockDisplay::BlockDisplay(QObject *parent) {

}

BlockDisplay::BlockDisplay(int blocknum, long time, QObject *parent) : BlockDisplay(parent) {
    this->m_blocknumber = blocknum;
    this->m_time = time;
}

BlockDisplay::BlockDisplay(const BlockDisplay &other, QObject *parent) : BlockDisplay(parent) {
    this->m_blocknumber = other.blockNumber();
    this->m_time = other.time();
}

BlockDisplay::~BlockDisplay() {

}

int BlockDisplay::blockNumber() const {
    return this->m_blocknumber;
}

long BlockDisplay::time() const {
    return this->m_time;
}

