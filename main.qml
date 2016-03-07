import QtQuick 2.5
import QtQuick.Controls 1.4

//import "content"
import BlockExplorer 1.0


ApplicationWindow {
    id: root
    visible: true
    title: "ProtoBlock Explorer"
    property int margin: 10
    property string hash
    width: 800//mainLayout.implicitWidth + 2 * margin
    height: 600//mainLayout.implicitHeight + 2 * margin
    minimumWidth: mainLayout.Layout.minimumWidth + 2 * margin
    minimumHeight: mainLayout.Layout.minimumHeight + 2 * margin


    Loader{
        id: mainLoader
        anchors.fill: parent

        source: "NewMain.qml"


    }



}

