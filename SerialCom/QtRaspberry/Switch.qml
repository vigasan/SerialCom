import QtQuick 2.0

Item
{
    id: toggleswitch
    width: background.width;
    height: background.height

    signal released(bool value)
    signal switched(bool on)

    property bool on: false
    property bool toggleEnabled : true
    property color colore: "#39F724"

    property int backgroundWidth: 70
    property int backgroundHeight: 30

    function toggle()
    {
        if (toggleswitch.state == "on")
            toggleswitch.state = "off";
        else
            toggleswitch.state = "on";
        switched(toggleswitch.on);
    }

    function setStatus(value)
    {
        if (value === "on")
            toggleswitch.state = "on";
        else
            toggleswitch.state = "off";
    }

    function releaseSwitch()
    {
        if (knob.x == 1)
        {
            if (toggleswitch.state == "off")
            {
                toggleswitch.on = false;
                return;
            }
        }
        if (knob.x == 40)
        {
            if (toggleswitch.state == "on")
            {
                toggleswitch.on = true;
                return;
            }
        }
        toggle();
    }

    Rectangle
    {
        id: background
        width: backgroundWidth; height: backgroundHeight
        radius: height
        color: "#313E53"

        MouseArea
        {
            anchors.fill: parent;
            onClicked: toggle();
            enabled:toggleEnabled
        }

    }

    Rectangle
    {
        id: knob
        width: backgroundHeight; height: backgroundHeight
        radius: width
        gradient: Gradient
        {
            GradientStop { position: 0.0; color: "lightgray" }
            GradientStop { position: 1.0; color: "white" }
        }

        MouseArea
        {
            anchors.fill: parent
            enabled:toggleEnabled
            drag.target: knob; drag.axis: Drag.XAxis; drag.minimumX: 1; drag.maximumX: backgroundWidth-backgroundHeight //40
            onClicked:
            {
                toggle();
            }
            onReleased:
            {
                releaseSwitch();
                toggleswitch.released(on);
            }
        }
    }

    Rectangle
    {
        id: backgroundDisabled
        width: 70; height: 30
        radius: 30
        color: "#313E53"
        opacity: 0.8
        visible: !toggleEnabled
    }

    states:
    [
        State
        {
            name: "on"
            PropertyChanges { target: knob; x: backgroundWidth-backgroundHeight }
            PropertyChanges { target: toggleswitch; on: true }
            PropertyChanges { target: background; color: toggleswitch.colore }

        },
        State
        {
            name: "off"
            PropertyChanges { target: knob; x: 1 }
            PropertyChanges { target: toggleswitch; on: false }
            PropertyChanges { target: background; color: "#313E53" }
        }
    ]

    transitions:
    Transition
    {
        NumberAnimation { properties: "x"; easing.type: Easing.InOutQuad; duration: 200 }
    }
}

