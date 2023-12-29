import QtQuick
import second 1.0

import my.rej 1.0

import third 1.0

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

    Haha {

    }

    Connections{
        target: re
        function onSigreg(){
            console.log("11111111");
        }
    }

    Rejest{
    }

    ThridTst {

    }
}
