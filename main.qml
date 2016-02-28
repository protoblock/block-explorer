import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.2

import "content"

ApplicationWindow {
    visible: true
    title: "ProtoBlock Explorer"
    property int margin: 10
    width: mainLayout.implicitWidth + 2 * margin
    height: mainLayout.implicitHeight + 2 * margin
    minimumWidth: mainLayout.Layout.minimumWidth + 2 * margin
    minimumHeight: mainLayout.Layout.minimumHeight + 2 * margin

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
                             color: 'grey'
                             visible: list.activeFocus
                        }

                       highlightFollowsCurrentItem: true;

                        clip: true

                        delegate: blockdel

                        onCurrentItemChanged: console.log(currentIndex + 'selected')

                        Keys.onReturnPressed: console.log(currentIndex + 'enter')

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

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    list.currentIndex = index;
                    console.log("clicked " + index);
                    list.focus = true;
                    Keys.forwardTo = list  ;
                }
            }
        }
    }

}
    /*
    Blocks {
        id: blk
        anchors.fill: parent
        modelclass: mc
        bh: mc.blockheight
        //lm: mc
        //source: "blocks.qml"
    }

    /*
    signal prevPressed();
    signal nextPressed();

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: console.log("Open action triggered");
            }
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }
    */

    //ScrollView {
    //    anchors.fill: parent

    //    Keys.forwardTo: list

/*
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
            //anchors.fill: parent
            //anchors.top: tophead.bottom
            Layout.fillHeight: true
            //cellWidth: 200 //parent.width;
            //cellHeight: 15
            delegate: gridComp

            highlight: Rectangle {
                 color: 'grey'
            }

            // 2016-02-18 - Commented out by Ryan to surpress errors
            //onCurrentItemChanged: console.log(model.get(list.currentIndex).index + ' selected')


            focus: true

            // 2016-02-19 - Ryan commented out this line to prevent wrapping from 1 to blockheight
            // on scroll up
            //keyNavigationWraps: true

            Keys.onReturnPressed: currentItem.clicked()

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
*/
/*
    TabView {
        anchors.fill: parent

        Tab {
            title: "Blocks"
            Item {
                anchors.centerIn: parent

                TextArea {
                    id: block
                    x: 107
                    y: 62
                    width: 392
                    height: 327
                    visible: true
                    frameVisible: true
                    tabChangesFocus: false
                    readOnly: true
                    text: Display.blockHeader
                }

                Button {
                    id: previous
                    x: 240
                    y: 400
                    onClicked: mainWindow.prevPressed()
                    text: qsTr("Previous")
                }

                Button {
                    id: next
                    x: 340
                    y: 400
                    onClicked: mainWindow.nextPressed()
                    text: qsTr("Next")
                }

            }
        }
        Tab {
            title: "Transactions"
            Item {
                anchors.centerIn: parent

                TextArea {
                    id: transactions
                    x: 107
                    y: 62
                    width: 392
                    height: 327
                    visible: true
                    frameVisible: true
                    tabChangesFocus: false
                    readOnly: true
                    text: Display.transactions
                }

                Button {
                    id: transaction_previous
                    x: 240
                    y: 400
                    onClicked: mainWindow.prevPressed()
                    text: qsTr("Previous")
                }

                Button {
                    id: transaction_next
                    x: 340
                    y: 400
                    onClicked: mainWindow.nextPressed()
                    text: qsTr("Next")
                }
            }
        }
        Tab {
            title: "State"
            anchors.centerIn: parent

            Rectangle {
                color: "green"
            }
        }
    }
*/

    /*
    Component {
        id: gridComp
        Row {
            Text {
                text: blocknum + " "

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        list.currentIndex = index;
                        //messageDialog.show(blocknum);

                        var component = Qt.createComponent("qrc:/detail.qml")
                        var window    = component.createObject(mainWindow)
                        window.show()
                        mainWindow.hide()
                    }
                }
            }
            //Column {
                Text {
                    text: time + " "
                }
            //}
        }
    }

    MessageDialog {
        id: messageDialog
        title: qsTr("May I have your attention, please?")

        function show(caption) {
            messageDialog.text = caption;
            messageDialog.open();
        }
    }
    */
//}

