import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import game
import enums

Window {
    width: 1024
    height: 720
    visible: true
    title: qsTr("Checkers")

    property int spot_size: 60
    property int decor_spacing: 9
    property int decor_margin: spot_size / 60 * 40


    Rectangle {
       id: board_rect
       width: 8 * spot_size + 6
       height: width
       color: "red"
       anchors.centerIn: parent
    }

    Image {
        id: board
        width: 8 * spot_size
        height: 8 * spot_size
        source: "qrc:/img/board.png"
        objectName: "board"
        anchors.centerIn: board_rect
    }

//    // NOTE Use this if no board image
//    Grid {
//        id: board
//        objectName: "board"
//        columns: 8
//        rows: 8
//        spacing: 0
//        anchors.centerIn: board_rect
//        Repeater {
//            model: 64
//            Rectangle {
//                width: spot_size
//                height: spot_size
//                color: ((index / 8) + index) & 1 ? "black" : "white"
//            }
//        }
//    }

    ColumnLayout {
        anchors.right: board.left
        anchors.top: board.top
        anchors.rightMargin: 100
        anchors.topMargin: 16

        Button {
            id: start_button
            text: "Start"
            onClicked: {
                Game.start();
            }
            enabled: Game.status !== Enums.GOING
        }
        Button {
            id: stop_button
            text: "Stop"
            onClicked: {
                Game.stop();
            }
            enabled: Game.status === Enums.GOING
        }
    }

    Label {
        id: status_lbl
        text: ""
        font.pointSize: 14
        anchors.horizontalCenter: board.horizontalCenter
        anchors.bottom: board.top
        anchors.bottomMargin: 69
    }

    Label {
        property var msg: [
            "nice", "good move", "nailed it", "wow",
            "brilliant", "what a move", "finish him", "cool",
            "awesome", "doing great", ":D", "congratulations",
            "you're a beast", "not bad",
        ]

        id: msg_lbl
        text: msg[Math.floor(msg.length * Math.random())];
        color: "red"
        font.pointSize: 13
        anchors.horizontalCenter: board.horizontalCenter
        anchors.bottom: board.top
        anchors.bottomMargin: 92
        visible: timer.running

        Timer {
            id: timer
            running: false
            repeat: false
            interval: 2500
            onTriggered: msg_lbl.text = parent.msg[Math.floor(parent.msg.length * Math.random())];
        }
    }

    Connections {
        target: Game

        property int i: 0;

        function onStatusChanged(turn)
        {
            if (++i % 5 == 0) {
                if (!timer.running)
                    timer.start();
            }

            switch (Game.status) {
                case Enums.GOING:   status_lbl.text = (turn === Enums.BLACK ? "Black" : "White") + "'s turn"; break;
                case Enums.WIN:     status_lbl.text = (turn === Enums.BLACK ? "White" : "Black") + " won"; break;
                case Enums.DRAW:    status_lbl.text = "Draw"; break;
                case Enums.STOPPED: status_lbl.text = "Stopped"; break;
            }
        }
    }

    Row {
        spacing: decor_spacing
        anchors.bottom: board.top
        anchors.margins: decor_margin
        anchors.horizontalCenter: board.horizontalCenter
        Decor {}
    }

    Row {
        spacing: decor_spacing
        anchors.top: board.bottom
        anchors.margins: decor_margin
        anchors.horizontalCenter: board.horizontalCenter
        Decor {}
    }

    Column {
        spacing: decor_spacing
        anchors.right: board.left
        anchors.margins: decor_margin
        anchors.verticalCenter: board.verticalCenter
        Decor {}
    }

    Column {
        spacing: decor_spacing
        anchors.left: board.right
        anchors.margins: decor_margin
        anchors.verticalCenter: board.verticalCenter
        Decor {}
    }
}
