import QtQuick
import QtQuick.Controls

Window {
    visibility: Window.FullScreen
    width: Screen.width
    height: Screen.height
    visible: true
    title: "Hello Smart Home"
    color: "black"

    Rectangle {
        anchors.centerIn: parent
        width: 300
        height: 200
        color: "#ff0000" // Red box
        radius: 10

        Text {
            anchors.centerIn: parent
            text: "Hello Docker!"
            color: "white"
            font.pixelSize: 24
        }
    }
}