#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QObject>
#include <QMutex>
#include <QtSerialPort/QSerialPort>
#include "frame.h"
#include <QQueue>


class SerialWorker : public QObject
{
    Q_OBJECT
public:
    explicit SerialWorker(QQueue<Frame*> *outFrameQueue, QObject *parent = nullptr);
    ~SerialWorker();
    void requestWork();
    void abort();

private:
    static const int RCV_ST_IDLE = 0;
    static const int RCV_ST_CMD = 1;
    static const int RCV_ST_DATA_LENGTH = 2;
    static const int RCV_ST_DATA = 3;
    static const int RCV_ST_CHECKSUM = 4;

    bool _abort;
    bool _working;
    QMutex mutex;
    QSerialPort *m_Serial;
    QQueue<Frame*> *m_outFrameQueue;
    quint8 calculateChecksum(QByteArray buffer);
    void sendData(Frame *frame);

signals:
    void workRequested();
    void valueChanged(const QString &value);
    void finished();
    void frameReceived(Frame *frame);

public slots:
    void doWork();
    void sendUint8(quint8 cmd, quint8 data);
    void sendUint16(quint8 cmd, quint16 data);
    void sendUint32(quint8 cmd, quint32 data);
    void sendFrame(Frame *frame);

};

#endif // SERIALWORKER_H
