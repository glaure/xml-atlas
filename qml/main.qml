import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3

Window {
    visible: true
    width: 300
    height: 300

    ColumnLayout {
        anchors.fill: parent

        CheckBox {
            text: "Checkbox for " + "AB"
        }
        Button {
            text: "Hi!"
        }
    }
}