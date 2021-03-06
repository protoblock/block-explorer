#ifndef TEST_H
#define TEST_H

#include <QObject>

#include <blockchain.h>

namespace fantasybit_bx {

class Display : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString blockHeader READ blockHeader WRITE setBlockHeader NOTIFY blockHeaderChanged)
    Q_PROPERTY(QString transactions READ transactions WRITE setTransactions NOTIFY transactionsChanged)

public:
    explicit Display(QObject *parent = 0);

    QString blockHeader() const;
    void setBlockHeader(QString txt);

    QString transactions() const;
    void setTransactions(QString txt);

    QString getTransactionsString();

signals:
    void blockHeaderChanged();
    void transactionsChanged();

public slots:
    void prevPressedSlot();
    void nextPressedSlot();

protected:
    QString m_block = "Default String";
    QString m_transactions = "Default String";
    Blockchain bc{};
};

}

#endif // TEST_H
