#ifndef TEST_H
#define TEST_H

#include <QObject>

class Test : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

public:
    explicit Test(QObject *parent = 0);
    QString text() const;
    void setText(QString txt);

signals:
    void textChanged();

public slots:
    void prevPressedSlot();
    void nextPressedSlot();

protected:
    QString m_text = "Default String";
};

#endif // TEST_H
