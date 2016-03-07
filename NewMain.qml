import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.2

import "content"
import BlockExplorer 1.0

Rectangle {
    visible: true
//    title: "ProtoBlock Explorer"
    property int margin: 10

    width: 800//mainLayout.implicitWidth + 2 * margin
    height: 600//mainLayout.implicitHeight + 2 * margin
//    minimumWidth`: mainLayout.Layout.minimumWidth + 2 * margin
//    minimumHeight: mainLayout.Layout.minimumHeight + 2 * margin

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: margin

        ColumnLayout {
            anchors.fill: parent

            Button {
                text: "run"
            }
        }

        GroupBox {
            id: rowBox
            title: "Block Chain"
            Layout.fillWidth: true

            ColumnLayout {
                id: rowLayout
                anchors.fill: parent
                anchors.margins: margin

                Labeltext {
                    Layout.fillWidth: true
                    pLable: "Block Height"
                    pText: mc.blockheight
                }

                Labeltext {
                    Layout.fillWidth: true
                    pLable: "Start Block"
                    pText: mc.firstblock
                }
            }
        }

        GroupBox {
            id: listbox
            title: "blocks"
            Layout.fillWidth: true
            Layout.fillHeight: true
            implicitWidth: rowBox.implicitWidth
            //anchors.fill: parent

            ColumnLayout {
                anchors.fill: parent
                //anchors.margins: margin
                //Layout.fillHeight: true

                ScrollView {
                    anchors.fill: parent
                    Keys.forwardTo: list
                    flickableItem.interactive: true
                    ListView {
                        id: list
                        model: mc
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        anchors.fill: parent
                        focus: true

                        //Layout.preferredHeight: 3000
                        //Layout.fillHeight: true
                        highlight: Rectangle {
                             color: 'lightgrey'
                             visible: list.activeFocus
                        }

                        //highlightFollowsCurrentItem: true;

                        clip: true

                        delegate: blockdel

                        onCurrentItemChanged: {
                            console.log(currentIndex + 'selected')
                        }

                        Keys.onReturnPressed: console.log(currentIndex + 'enter')

                        Component.onCompleted: { forceActiveFocus(); }
                        Keys.onPressed: {
                            console.log("onPressed " + event.key);
                            if (event.key == Qt.Key_Up || event.key == Qt.Key_Down) {
                                if (event.key == Qt.Key_Down && currentIndex < count - 1)
                                    currentIndex++;
                                else if (event.key == Qt.Key_Up && currentIndex > 0)
                                    currentIndex--;

                                event.accepted = true;
                                return;
                            }

                            var pageDown = currentIndex+10;
                            var pageUp = currentIndex-10;
                            if ( event.key === Qt.Key_PageDown && event.modifiers === Qt.NoModifier) {
                                currentIndex = pageDown >= count ? count-1 : pageDown;
                                event.accepted = true;
                            }
                            else if (event.key === Qt.Key_PageUp && event.modifiers === Qt.NoModifier) {
                                currentIndex = pageUp < 0 ? 0 : pageUp;
                                event.accepted = true;
                            }
                        }

                    }

                }
            }

        }
    }

    Component {
        id: blockdel

        Label {
            //Layout.fillWidth: true
            text: blocknum + " " + time
            color: ListView.isCurrentItem ? "Blue" : "Black"
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    list.currentIndex = index;
                    console.log("clicked " + index);
                    list.focus = true;
                    Keys.forwardTo = list  ;
                    CurrentFocusedBlock.num = blocknum

                    mainLoader.source = "NodeHash.qml"
                    //list.highlightItem = list.currentItem;
                }
            }
        }
    }

}
