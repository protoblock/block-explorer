import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

Item {
    width: 640
    height: 480

    property alias button1: previous
    property alias button2: next
    property alias block: block

    RowLayout {
        anchors.verticalCenterOffset: 214
        anchors.horizontalCenterOffset: 1
        anchors.centerIn: parent

        Button {
            id: previous
            text: qsTr("Previous")
        }

        Button {
            id: next
            text: qsTr("Next")
        }
    }

    TextArea {
        id: block
        x: 72
        y: 8
        width: 498
        height: 421
        visible: true
        frameVisible: true
        tabChangesFocus: false
        readOnly: true
    }
}

