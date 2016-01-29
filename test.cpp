#include "test.h"

#include <QDebug>

using namespace std;

Test::Test(QObject *parent) : QObject(parent)
{
    bc.SeekToFirst();
    QString qstr = QString::fromStdString(bc.GetCurrentBlock().signedhead().DebugString());
    this->setText(qstr);
}

void Test::prevPressedSlot() {
    //qDebug() << "Test::prevPressedSlot called" << endl;
    //this->setText("prevPressedSlot");

    bc.Prev();
    QString qstr = QString::fromStdString(bc.GetCurrentBlock().signedhead().DebugString());
    this->setText(qstr);
}

void Test::nextPressedSlot() {
    //qDebug() << "Test::nextPressedSlot called" << endl;
    //this->setText("nextPressedSlot");

    bc.Next();
    QString qstr = QString::fromStdString(bc.GetCurrentBlock().signedhead().DebugString());
    this->setText(qstr);
}

QString Test::text() const {
    return this->m_text;
}

void Test::setText(QString txt) {
    this->m_text = txt;
    emit textChanged();
}
