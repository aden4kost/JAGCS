import QtQuick 2.6
import QtQuick.Layouts 1.3
import JAGCS 1.0

import Industrial.Controls 1.0 as Controls
import "qrc:/Common" as Common

Controls.Frame {
    id: linkView

    property LinkProvider provider: LinkProvider {}

    property bool minimized: true
    default property alias content: contentColumn.children

    signal removeRequest()
    signal minimize(bool minimize)

    Common.MvBinding {
        when: nameField.activeFocus
        viewModelProperty: provider.name;
        viewProperty: nameField.text
    }

    implicitWidth: column.implicitWidth + sizings.margins * 2
    implicitHeight: column.implicitHeight + sizings.margins * 2

    ColumnLayout {
        id: column
        anchors.fill: parent
        spacing: sizings.spacing

        Controls.Label {
            text: provider.name
            visible: minimized
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }

        RowLayout {
            spacing: sizings.spacing
            visible: minimized

            Controls.Label {
                text: {
                    switch (provider.type) {
                    case LinkDescription.Serial:
                        return qsTr("Serial");
                    case LinkDescription.Udp:
                        return qsTr("UDP");
                    case LinkDescription.Tcp:
                        return qsTr("TCP");
                    case LinkDescription.Bluetooth:
                        return qsTr("Bluetooth");
                    default:
                        return qsTr("Unknown");
                    }
                }
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }

            Controls.Label {
                text: provider.protocol.length ? provider.protocol : "-"
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }
        }

        ColumnLayout {
            id: contentColumn
            spacing: sizings.spacing
            visible: !minimized

            Controls.TextField {
                id: nameField
                labelText: qsTr("Name")
                readOnly: minimized
                horizontalAlignment: Text.AlignHCenter
                Layout.leftMargin: connectButton.width
                Layout.rightMargin: minimizeButton.width
                Layout.fillWidth: true
            }

            Controls.ComboBox {
                id: protocolBox
                labelText: qsTr("Protocol")
                visible: !minimized
                model: provider.availableProtocols
                onDisplayTextChanged: provider.setProtocol(displayText)
                Layout.fillWidth: true
            }
        }

        RecvSentRow {
            id: recvSent
            Layout.fillWidth: true
        }

        Controls.DelayButton {
            flat: true
            iconSource: "qrc:/icons/remove.svg"
            text: qsTr("Remove");
            onClicked: removeRequest()
            Layout.fillWidth: true
        }
    }

    Controls.Button {
        id: connectButton
        anchors.left: parent.left
        anchors.top: parent.top
        flat: true
        iconSource: provider.connected ? "qrc:/icons/arrow_up.svg" : "qrc:/icons/arrow_down.svg"
        iconColor: provider.connected ? customPalette.positiveColor : customPalette.sunkenColor
        tipText: provider.connected ? qsTr("Disconnect") : qsTr("Connect");
        onClicked: provider.setConnected(!provider.connected)
    }

    Controls.Button {
        id: minimizeButton
        anchors.right: parent.right
        anchors.top: parent.top
        flat: true
        iconSource: minimized ? "qrc:/ui/down.svg" : "qrc:/ui/up.svg"
        tipText: minimized ? qsTr("Maximize") : qsTr("Minimize");
        onClicked: minimize(!minimized)
    }
}
