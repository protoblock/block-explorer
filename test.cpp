#include "test.h"

#include <QDebug>

using namespace std;

Test::Test(QObject *parent) : QObject(parent)
{
}

void Test::prevPressedSlot() {
    qDebug() << "Test::prevPressedSlot called" << endl;
    this->setText("prevPressedSlot");
}

void Test::nextPressedSlot() {
    qDebug() << "Test::nextPressedSlot called" << endl;
    this->setText("nextPressedSlot");
}

QString Test::text() const {
    return this->m_text;
}

void Test::setText(QString txt) {
    this->m_text = txt;
    emit textChanged();
}
