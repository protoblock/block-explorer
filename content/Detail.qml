import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.2

Item {
    id: root
    anchors.fill: parent
    visible: true

    signal closed

    ColumnLayout {
        Text {
            text: "detail.qml"
        }

        Button {
            text: "Back"
            onClicked:  root.closed()
        }
    }
}

