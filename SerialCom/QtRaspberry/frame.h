#ifndef FRAME_H
#define FRAME_H

#include <QObject>

class Frame : public QObject
{
    Q_OBJECT

public:
    static const quint8 FRAME_START = 0x8A;
    static const quint8 FRAME_ESCAPE_CHAR = 0x8B;
    static const quint8 FRAME_XOR_CHAR = 0x20;
    static const quint8 FRAME_NUM_EXTRA_BYTES = 4;


    static const int INDEX_START_OF_FRAME = 0;
    static const int INDEX_CMD = 1;
    static const int INDEX_DATA_LENGTH = 2;
    static const int INDEX_FIRST_DATA_BYTE = 3;
private:
    QByteArray m_buffer;


public:
    explicit Frame(QObject *parent = nullptr);
    //explicit Frame(const Frame &obj);
    explicit Frame(quint8 cmd, quint8 data, QObject *parent = nullptr);
    explicit Frame(quint8 cmd, qint8 data, QObject *parent = nullptr);
    explicit Frame(quint8 cmd, quint16 data, QObject *parent = nullptr);
    explicit Frame(quint8 cmd, qint16 data, QObject *parent = nullptr);
    explicit Frame(quint8 cmd, quint32 data, QObject *parent = nullptr);
    explicit Frame(quint8 cmd, qint32 data, QObject *parent = nullptr);
    explicit Frame(quint8 cmd, QByteArray data, QObject *parent = nullptr);
     ~Frame();

    static quint16 makeWord(quint8 l, quint8 h);
    static quint32 makeDWord(quint16 l, quint16 h);


    quint8  GetCmd();
    quint8  GetDataLength();
    quint8  GetIndexedByte(int index);
    quint8  GetUByte();
    quint8  GetUByte(int index);
    qint8   GetSByte();
    quint16 GetUInt16();
    quint16 GetUInt16(int index);
    qint16  GetInt16();
    quint32 GetUInt32();
    quint32 GetUInt32(int index);
    qint32  GetInt32();
    quint8  CalculateChecksum();
    void    Clear();
    void    AddByte(quint8 data);
    QByteArray GetBuffer();

signals:

public slots:



};

#endif // FRAME_H
