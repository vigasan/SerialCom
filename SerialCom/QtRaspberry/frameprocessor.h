#ifndef FRAMEPROCESSOR_H
#define FRAMEPROCESSOR_H

#include <QObject>
#include "frame.h"
#include "serialworker.h"
#include <QQueue>

class FrameProcessor : public QObject
{
    static const quint8 CMD_BUTTON_1 = 1;
    static const quint8 CMD_BUTTON_2 = 2;
    static const quint8 CMD_LED_GREEN = 3;
    static const quint8 CMD_PWM_LED_R = 4;
    static const quint8 CMD_PWM_LED_G = 5;
    static const quint8 CMD_PWM_LED_B = 6;
    static const quint8 CMD_ADC_INPUT = 7;
    static const quint8 CMD_ADC_ENABLE = 8;

    Q_OBJECT
public:
    explicit FrameProcessor(QQueue<Frame*> *outFrameQueue, QObject *parent = nullptr);

private:
    //SerialWorker *m_serialWorker;
    QQueue<Frame*> *m_outFrameQueue;

signals:
    void changedInput1(int value);
    void changedInput2(int value);
    void changedAdc(int value);

public slots:
    void FrameIncoming(Frame *frame);

    void setPwm(quint8 color, quint8 value);
    void enableAdc(quint8 enable);
};

#endif // FRAMEPROCESSOR_H
