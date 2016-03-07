#include "currentheaderblock.h"
#include <QDebug>

CurrentHeaderBlock::CurrentHeaderBlock(QObject *parent) : QObject(parent)
{

}

int CurrentHeaderBlock::version() const
{
    return m_version;
}

void CurrentHeaderBlock::setVersion(const int &version)
{
    if (m_version == version )
        return;
    m_version = version;
}

int CurrentHeaderBlock::num() const
{
    qDebug() << "get num" << m_num;

    return m_num;
}

void CurrentHeaderBlock::setNum(const int &num)
{
    qDebug() << "set num" <<  num;
    if (m_num == num )
        return;
    m_num = num;
}

QString CurrentHeaderBlock::prev_id() const
{
    return m_prev_id;
}

void CurrentHeaderBlock::setPrev_id(const QString &prev_id)
{
    if (m_prev_id == prev_id )
        return;
    m_prev_id = prev_id;

}

QDateTime CurrentHeaderBlock::timestamp() const
{
    return m_timestamp;
}

void CurrentHeaderBlock::setTimestamp(const QDateTime &timestamp)
{
    if (m_timestamp == timestamp )
        return;
    m_timestamp = timestamp;
    emit blockHeaderChanged();
}

QString CurrentHeaderBlock::genPK() const
{
    return m_genPK;
}

void CurrentHeaderBlock::setGenPK(const QString &genPK)
{
    if (m_genPK == genPK )
        return;
    m_genPK = genPK;
}

QString CurrentHeaderBlock::genSIG() const
{
    return m_genSIG;
}

void CurrentHeaderBlock::setGenSIG(const QString &genSIG)
{
    if (m_genSIG == genSIG )
        return;
    m_genSIG = genSIG;
}

QString CurrentHeaderBlock::transactionID() const
{
    return m_transactionID;
}

void CurrentHeaderBlock::setTransactionID(const QString &transactionID)
{
    if (m_transactionID == transactionID )
        return;
    m_transactionID = transactionID;
}


