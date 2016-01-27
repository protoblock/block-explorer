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
    utils.cpp \
    blockchain.cpp \
    actions.cpp

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

    LIBS += -lfc
  LIBS+= -lssl \
         -lcrypto

}

win32 {
   LIBS+= -L$$PWD/libwin64

    CONFIG(debug, debug|release) {
       LIBS+= -llibprotobufd \
              -lleveldbd \
              -llibeay32 \
              -lssleay32 \
              -lfcd

    }
    CONFIG(release, debug|release) {
       LIBS+= -llibprotobuf \
              -lleveldb \
              -llibeay32 \
              -lssleay32 \
              -lfc
    }

    BOOST_DIR = C:/work/boost
    LIBS += -L$${BOOST_DIR}/lib64-msvc-12.0
    LIBS += -lboost_log-vc120-1_55
    INCLUDEPATH += $${BOOST_DIR}
}

INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/generated

HEADERS += \
    utils.h \
    blockchain.h \
    actions.h
