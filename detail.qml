import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.2

Item {
    id: detailWindow
    visible: true

    ColumnLayout {
        Text {
            text: "detail.qml"
        }

        Button {
            text: "Back"

            onClicked: {
                ld.source = "blocks.qml"
            }
        }
    }
}

