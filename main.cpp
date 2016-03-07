#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QtQml>

#include <display.h>
#include <modelclass.h>
#include <actions.h>
#include <string>
#include <createmeta.h>

#include "currentheaderblock.h"
using namespace std;

using namespace fantasybit;
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;


    ModelClass mc{};
    mc.init();

    engine.rootContext()->setContextProperty("mc", &mc);

    qmlRegisterUncreatableType<ModelClass>("satoshifantasy.com",1,1,"ModelClass","modelclass singleton");

    qmlRegisterSingletonType<CurrentHeaderBlock>("BlockExplorer",1,0,"CurrentFocusedBlock", currentHeaderBlockSingle);



    engine.load(QUrl(QStringLiteral("main.qml")));

    return app.exec();

}

