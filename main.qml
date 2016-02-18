import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.2



ApplicationWindow {
    id: mainWindow
    visible: true
    width: 640
    height: 480
    title: qsTr("Block Explorer")

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

    //ScrollView {
    //    anchors.fill: parent

    //    Keys.forwardTo: list

        ListView {
            id: list
            model: mc
            anchors.fill: parent
            delegate: Text {
                text: display
                MouseArea {
                    anchors.fill: parent
                    onClicked: list.currentIndex = index
                }

            }
 /*
                Component {
                //width: ListView.view.width
                //height: 40
                Text {
                    anchors.centerIn: parent

                    //font.pixelSize: 10

                    text: display
                }
                //Text {text: display}
//            }
*/
            highlight: Rectangle {
                 color: 'grey'
            }

            onCurrentItemChanged: console.log(model.get(list.currentIndex).index + ' selected')

            focus: true

            keyNavigationWraps: true

            //Keys.onReturnPressed: currentItem.clicked()

            Keys.onPressed: {
                var pageDown = currentIndex+10;
                var pageUp = currentIndex-10;
                if (event.key == Qt.Key_PageDown && event.modifiers == Qt.NoModifier) {
                    currentIndex = pageDown >= count ? count-1 : pageDown;
                    event.accepted = true;
                }
                if (event.key == Qt.Key_PageUp && event.modifiers == Qt.NoModifier) {
                    currentIndex = pageUp < 0 ? 0 : pageUp;
                    event.accepted = true;
                }
            }

        }
   // }

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
    MessageDialog {
        id: messageDialog
        title: qsTr("May I have your attention, please?")

        function show(caption) {
            messageDialog.text = caption;
            messageDialog.open();
        }
    }
}

