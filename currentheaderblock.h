#ifndef CURRENTHEADERBLOCK_H
#define CURRENTHEADERBLOCK_H

#include <QObject>
#include <QJSEngine>
#include <QQmlEngine>
#include <QDateTime>

class CurrentHeaderBlock : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int version READ version WRITE setVersion)
    Q_PROPERTY(int num READ version WRITE setNum)
    Q_PROPERTY(QString prev_id READ prev_id WRITE setPrev_id )
    Q_PROPERTY(QDateTime timestamp READ timestamp WRITE setTimestamp)
    Q_PROPERTY(QString genPK READ genPK WRITE setGenPK )
    Q_PROPERTY(QString genSIG READ genSIG WRITE setGenSIG )
    Q_PROPERTY(QString transactionID READ transactionID WRITE setTransactionID )

public:
    explicit CurrentHeaderBlock(QObject *parent = 0);

    int version()const;
    void setVersion(const int &version);

    int num()const;
    void setNum(const int &num);

    QString prev_id()const;
    void setPrev_id(const QString &prev_id);

    QDateTime timestamp()const;
    void setTimestamp(const QDateTime &timestamp);

    QString genPK()const;
    void setGenPK(const QString &genPK);


    QString genSIG()const;
    void setGenSIG(const QString &genSIG);

    QString transactionID() const;
    void setTransactionID(const QString &transactionID);

signals:
    void blockHeaderChanged();

private:
    int m_version;
    int m_num;
    QString m_prev_id;
    QDateTime m_timestamp;
    QString m_genPK;
    QString m_genSIG;
    QString m_transactionID;

};

static QObject *currentHeaderBlockSingle(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    CurrentHeaderBlock *example = new CurrentHeaderBlock();
    return example;
}


#endif // CURRENTHEADERBLOCK_H
