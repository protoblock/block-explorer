import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4


RowLayout {
    property string pLable : "Lableltext.pLable"
    property string pText : "Labeltext.pText"
    id: item
    //anchors.fill: parent

    Label {
        //Layout.fillWidth: true
        text: item.pLable + ": "
    }

    Label {
        //Layout.fillWidth: true
        text: item.pText
    }

}

