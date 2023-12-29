import QtQuick
import com.Second 1.0
Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Item {
        anchors.fill: parent
        Test{
            anchors.fill: parent
        }
    }

    ElementPro {

    }

}
