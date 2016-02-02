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

    MessageDialog {
        id: messageDialog
        title: qsTr("May I have your attention, please?")

        function show(caption) {
            messageDialog.text = caption;
            messageDialog.open();
        }
    }
}

