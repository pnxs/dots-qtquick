import QtQuick
import QtQuick.Controls.Basic

ApplicationWindow {
    width: 1024
    height: 600
    visible: true
    color: "#1f1f1f"
    title: qsTr("dots-qtquick")

    DotsField {
        typeName: "RoundtripData"
    }
    
}
