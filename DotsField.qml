import QtQuick

Rectangle {
    property string typeName;

    color: "#b1b1b1"

    DotsSubscription {
        name: typeName
        onEventReceived: function(event) {
            textField.text = event.id;
        }
    }

    Text {
        id: label
        text: "DOTS Type " + typeName + ": "
        color: "#f0f0f0"
    }

    Text {
        id: textField

        anchors.left: label.right
        anchors.verticalCenter: label.verticalCenter

        text: "<waiting for update>"
        color: "#f0f0f0"
    }
}

