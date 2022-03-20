import QtQuick 2.14
import QtQuick.Window 2.14

// For the singleton instance
import com.raspberrypi.SerialCom 1.0

Window
{
    id: rootPage
    visible: true
    width: 800
    height: 480
    //color: "#233343"
    color: "black"


    readonly property int cmd_BUTTON_1:     1
    readonly property int cmd_BUTTON_2:     2
    readonly property int cmd_LED_GREEN:    3
    readonly property int cmd_PWM_LED_R:    4
    readonly property int cmd_PWM_LED_G:    5
    readonly property int cmd_PWM_LED_B:    6
    readonly property int cmd_ADC_INPUT:    7
    readonly property int cmd_ADC_ENABLE:   8

    FontLoader
    {
       id: fontHelvetica
       source:"qrc:/images/helvetica.ttf"
    }

    // Rectangle Container for Inputs
    Rectangle
    {
        id: rectInputs
        width: 150
        height: 460
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 10
        color: "transparent"
        border.color: "gray"
        border.width: 2
        radius: 10

        Text
        {
          text: qsTr("INPUTS")
          anchors.horizontalCenter: parent.horizontalCenter
          anchors.top: parent.top
          anchors.topMargin: 10
          color: "white"
          font.family: fontHelvetica.name
          font.pointSize: 20
        }

        Rectangle
        {
          id: rectCnt1
          width: 80
          height: width
          radius: width / 2
          anchors.horizontalCenter: parent.horizontalCenter
          anchors.top: parent.top
          anchors.topMargin: 90
          color: "transparent"
          border.width: 2
          border.color: "green"

          Rectangle
          {
              id: inctrl1
              width: 60
              height: width
              radius: width / 2
              anchors.centerIn: parent
              color: "transparent"
          }
        }

        Text
        {
          text: "IN 1"
          anchors.horizontalCenter: parent.horizontalCenter
          anchors.top: rectCnt1.bottom
          anchors.topMargin: 10
          color: "white"
          font.family: fontHelvetica.name
          font.pointSize: 15
        }

      Rectangle
      {
          id: rectCnt2
          width: 80
          height: width
          radius: width / 2
          anchors.horizontalCenter: parent.horizontalCenter
          anchors.top: rectCnt1.bottom
          anchors.topMargin: 60
          color: "transparent"
          border.width: 2
          border.color: "green"

          Rectangle
          {
              id: inctrl2
              width: 60
              height: width
              radius: width / 2
              anchors.centerIn: parent
              color: "transparent"
          }
      }

      Text
      {
          text: "IN 2"
          anchors.horizontalCenter: parent.horizontalCenter
          anchors.top: rectCnt2.bottom
          anchors.topMargin: 10
          color: "white"
          font.family: fontHelvetica.name
          font.pointSize: 15
      }



    }

    // Rectangle Container for Output
    Rectangle
    {
        id: rectOutputs
        width: 620
        height: 230
        anchors.left: rectInputs.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 10
        color: "transparent"
        border.color: "gray"
        border.width: 2
        radius: 10

        Text
        {
            text: qsTr("ANALOG")
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.top: parent.top
            anchors.topMargin: 10
            color: "white"
            font.family: fontHelvetica.name
            font.pointSize: 20
        }

        Switch
        {
            id: ledSwitch
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 70
            backgroundHeight: 50
            backgroundWidth: 110
            onSwitched:
            {
                if(on == true)   
                    FrameProcessorSingleton.enableAdc(1);
                else
                    FrameProcessorSingleton.enableAdc(0);
            }
        }

        ItemParam
        {
           id: gaugeAdc
           width: parent.height * 0.95
           anchors.horizontalCenter: parent.horizontalCenter
           anchors.verticalCenter: parent.verticalCenter
           title: "ADC"
           unit: ""
           minValue: 0
           maxValue: 4095
           paramValue: 0
           regMinValue: 0
           regMaxValue: 0
        }
    }

    // Rectangle Container for PWM
    Rectangle
    {
        id: rectPwm
        width: 620
        height: 220
        anchors.left: rectInputs.right
        anchors.leftMargin: 10
        anchors.top: rectOutputs.bottom
        anchors.topMargin: 10
        color: "transparent"
        border.color: "gray"
        border.width: 2
        radius: 10

        Text
        {
           text: qsTr("PWM")
           anchors.horizontalCenter: parent.horizontalCenter
           anchors.top: parent.top
           anchors.topMargin: 10
           color: "white"
           font.family: fontHelvetica.name
           font.pointSize: 20
        }

        Text
        {
           text: qsTr("R")
           anchors.right: sldRed.left
           anchors.rightMargin: 30
           anchors.verticalCenter: sldRed.verticalCenter
           color: "red"
           font.family: fontHelvetica.name
           font.pointSize: 25
        }

        Slider
        {
           id: sldRed
           anchors.horizontalCenter: parent.horizontalCenter
           anchors.top: parent.top
           anchors.topMargin: 50
           width: 450
           minimum: 0
           maximum: 255
           value: 0
           step: 1
           backgroundEmpty: "lightgray"
           backgroundFull: "red"
           pressCircle: "red"
           onValueChanged:
           {
               FrameProcessorSingleton.setPwm(cmd_PWM_LED_R, value);
           }
        }

       Text
       {
           text: sldRed.value
           anchors.left: sldRed.right
           anchors.leftMargin: 10
           anchors.verticalCenter: sldRed.verticalCenter
           color: "white"
           font.family: fontHelvetica.name
           font.pointSize: 25
       }

       Text
      {
          text: qsTr("G")
          anchors.right: sldGreen.left
          anchors.rightMargin: 30
          anchors.verticalCenter: sldGreen.verticalCenter
          color: "lightgreen"
          font.family: fontHelvetica.name
          font.pointSize: 25
      }

      Slider
      {
          id: sldGreen
          anchors.horizontalCenter: parent.horizontalCenter
          anchors.top: sldRed.bottom
          anchors.topMargin: 50
          width: 450
          minimum: 0
          maximum: 255
          value: 0
          step: 1
          backgroundEmpty: "lightgray"
          onValueChanged:
          {
              FrameProcessorSingleton.setPwm(cmd_PWM_LED_G, value);
          }
      }

      Text
      {
          text: sldGreen.value
          anchors.left: sldGreen.right
          anchors.leftMargin: 10
          anchors.verticalCenter: sldGreen.verticalCenter
          color: "white"
          font.family: fontHelvetica.name
          font.pointSize: 25
      }

      Text
      {
          text: qsTr("B")
          anchors.right: sldBlue.left
          anchors.rightMargin: 30
          anchors.verticalCenter: sldBlue.verticalCenter
          color: "blue"
          font.family: fontHelvetica.name
          font.pointSize: 25
      }

      Slider
      {
          id: sldBlue
          anchors.horizontalCenter: parent.horizontalCenter
          anchors.top: sldGreen.bottom
          anchors.topMargin: 50
          width: 450
          minimum: 0
          maximum: 255
          value: 0
          step: 1
          backgroundEmpty: "lightgray"
          backgroundFull: "blue"
          pressCircle: "blue"
          onValueChanged:
          {
              FrameProcessorSingleton.setPwm(cmd_PWM_LED_B, value);
          }
      }

      Text
      {
          text: sldBlue.value
          anchors.left: sldBlue.right
          anchors.leftMargin: 10
          anchors.verticalCenter: sldBlue.verticalCenter
          color: "white"
          font.family: fontHelvetica.name
          font.pointSize: 25
      }
    }



    Connections
    {
        target: FrameProcessorSingleton

        function onChangedInput1()
        {
            if(value === 1)
               inctrl1.color = "lightgray";
            else
               inctrl1.color = "transparent";
        }

        function onChangedInput2()
        {
            if(value === 1)
               inctrl2.color = "lightgray";
            else
               inctrl2.color = "transparent";
        }

        function onChangedAdc()
        {
            gaugeAdc.paramValue = value;
        }
    }
}
