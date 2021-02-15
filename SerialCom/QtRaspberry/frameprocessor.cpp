#include "frameprocessor.h"

FrameProcessor::FrameProcessor(QQueue<Frame*> *outFrameQueue, QObject *parent) : QObject(parent)
{
    //m_serialWorker = serialworker;
    m_outFrameQueue = outFrameQueue;
}

void FrameProcessor::FrameIncoming(Frame *frame)
{
    if (frame != nullptr)
    {
        quint8 cmd = frame->GetCmd();

        switch (cmd)
        {
            case CMD_BUTTON_1:
            {
                emit changedInput1(frame->GetUByte());
            } break;

            case CMD_BUTTON_2:
            {
                emit changedInput2(frame->GetUByte());
            } break;

            case CMD_ADC_INPUT:
            {
                emit changedAdc(frame->GetUInt16());
            } break;
        }
    }
}

void FrameProcessor::setPwm(quint8 color, quint8 value)
{
    Frame *frameToSend = new Frame(color, value);
    m_outFrameQueue->enqueue(frameToSend);
}

void FrameProcessor::enableAdc(quint8 enable)
{
    Frame *frameToSend = new Frame(CMD_ADC_ENABLE, enable);
    m_outFrameQueue->enqueue(frameToSend);

}
