#QT += core
#QT += qml quick

QT += quick
!no_desktop: QT += widgets

QT -= gui

DEFINES += BLOCK_EXPLORER_WRITE_FILLS
#DEFINES += TRACE
#DEFINES += JAYDESK
DEFINES += CUTE_FANTASY
#RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
#include(deployment.pri)

TARGET = blockexplorer
#CONFIG += console
#CONFIG -= app_bundle
CONFIG += c++11
#-stdlib=libc++

SOURCES += \
    #main.cpp \
    utils.cpp \
    blockchain.cpp \
    #actions.cpp \
    display.cpp \
#    modelclass.cpp \
    createmeta.cpp \
    currentheaderblock.cpp \
    createstate.cpp \
    ldbwriter.cpp \
    datastores.cpp \
    DistributionAlgo.cpp \
    limitbook.cpp \
    insidebook.cpp

OTHER_FILES += main.qml \
    content/Blocks.qml \
    content/Detail.qml



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
   LIBS+= -L"$$PWD/libwin64"

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

    BOOST_DIR = C:\local\boost_1_55_0
    LIBS += -L$${BOOST_DIR}/lib64-msvc-12.0
    LIBS += -lboost_log-vc120-1_55
    INCLUDEPATH += $${BOOST_DIR}
}

INCLUDEPATH += $$PWD/include
#INCLUDEPATH += $$PWD/generated

HEADERS += \
    utils.h \
    blockchain.h \
    #actions.h \
    display.h \
    #modelclass.h \
    createmeta.h \
    currentheaderblock.h \
    createstate.h \
    ldbwriter.h \
    datastores.h \
    DistributionAlgo.h \
    limitbook.h \
    insidebook.h \
    blockexplorer.h


DISTFILES += \
    content/Labeltext.qml \
    NewMain.qml \
    NodeHash.qml


contains(DEFINES, CUTE_FANTASY) {
    CONFIG += staticlib
    TEMPLATE = lib
}

!contains(DEFINES, CUTE_FANTASY) {
    TEMPLATE = app
    PROTOS = proto/*.proto
    include(Protobuf.pri)
    SOURCES += main.cpp
}
