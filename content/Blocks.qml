import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.2
//import satoshifantasy.com 1.1

import "../content"

Item {
    //property alias bh: tophead.text
    property alias modelclass: list.model
    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: margin

        ListView {
            id: list
            //model: mc
            Layout.fillHeight: true
            anchors.top: tophead.bottom
            anchors.left: parent.left
            anchors.right: parent.right


            anchors.bottom: parent.bottom
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

        Item {
            id: wrapper
            width: list.width
            height: 30

            //width: list.width
        //Row {
        //    id: rowview
            Item {
                id: txrow
                Text {
                    id: textdet
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    //height: 30
                    text: blocknum + " " + time
                }
            }

            Item {
                id: txdetail

                anchors.fill: parent
                Detail {
                    onClosed: wrapper.state = ""
                }

                opacity: 0

                MouseArea {
                    anchors.fill: parent
                }
            }

            MouseArea {
                anchors.fill: parent
                //onClicked: { wrapper.height = list.height }
                onClicked: parent.state = "expanded"
            }

        //}

            states: [
                State {
                    name: "expanded"

                    PropertyChanges { target: txrow; opacity: 0 }
                    PropertyChanges { target: txdetail; opacity: 1 }
                    PropertyChanges { target: wrapper; height: list.height }
                    PropertyChanges { target: list; contentY: wrapper.y; interactive: false }

                }
            ]

        }
    }
}

