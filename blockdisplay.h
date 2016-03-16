#ifndef BLOCKDISPLAY_H
#define BLOCKDISPLAY_H

#include <QObject>
#include <QMetaType>

class BlockDisplay : public QObject
{
    Q_OBJECT

public:
    BlockDisplay(QObject *parent = 0);
    BlockDisplay(int blocknum, long time, QObject *parent = 0);
    BlockDisplay(const BlockDisplay &other, QObject *parent = 0);
    ~BlockDisplay();

    int blockNumber() const;
    long time() const;

private:
    int m_blocknumber;
    long m_time;
};

Q_DECLARE_METATYPE(BlockDisplay)

#endif // BLOCKDISPLAY_H
