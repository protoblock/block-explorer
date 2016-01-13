QT += core
QT -= gui

TARGET = blockexplorer
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11
-stdlib=libc++

TEMPLATE = app

SOURCES += main.cpp \
    generated/ApiData.pb.cc \
    generated/ExData.pb.cc \
    generated/NameData.pb.cc \
    generated/ProtoData.pb.cc \
    generated/StaticData.pb.cc \
    generated/StatusData.pb.cc \
    utils.cpp

unix:macx {
    LIBS +=  -L$$PWD/libs \
              -L/usr/local/lib

    LIBS += -lleveldb

    LIBS += -lprotobuf

    LIBS += -lboost_log \
             -lboost_chrono\
             -lboost_coroutine \
             -lboost_filesystem \
             -lboost_system \
             -lboost_thread \
             -lboost_context \
              -lboost_date_time
}

INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/generated

HEADERS += \
    utils.h
