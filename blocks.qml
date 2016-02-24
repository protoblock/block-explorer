import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.2

Item {
    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: margin

        Text {
            //id: tophead
            text: mc.blockheight
        }

        ListView {
            id: list
            model: mc
            Layout.fillHeight: true
            delegate: gridComp

            highlight: Rectangle {
                 color: 'grey'
            }

            focus: true

            Keys.onPressed: {
                var pageDown = currentIndex+10;
                var pageUp = currentIndex-10;
                if (event.key === Qt.Key_PageDown && event.modifiers === Qt.NoModifier) {
                    currentIndex = pageDown >= count ? count-1 : pageDown;
                    event.accepted = true;
                }
                if (event.key === Qt.Key_PageUp && event.modifiers === Qt.NoModifier) {
                    currentIndex = pageUp < 0 ? 0 : pageUp;
                    event.accepted = true;
                }
            }

        }
    }

    Component {
        id: gridComp
        Row {
            Text {
                text: blocknum + " "

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        list.currentIndex = index;

                        /*
                        var component = Qt.createComponent("qrc:/detail.qml")
                        var window    = component.createObject(mainWindow)
                        window.show()
                        mainWindow.hide()
                        */

                        ld.setSource("detail.qml")
                    }
                }
            }
            Text {
                text: time + " "
            }
        }
    }
}

