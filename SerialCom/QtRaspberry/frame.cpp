#include "frame.h"
#include <QDebug>


Frame::Frame(QObject *parent) : QObject(parent)
{

}

Frame::Frame(quint8 cmd, quint8 data, QObject *parent) : QObject(parent)
{
    m_buffer[INDEX_START_OF_FRAME] = FRAME_START;
    m_buffer[INDEX_CMD] = cmd;
    m_buffer[INDEX_DATA_LENGTH] = 1;
    m_buffer[INDEX_FIRST_DATA_BYTE] = data;
    m_buffer += CalculateChecksum();
}

Frame::Frame(quint8 cmd, qint8 data, QObject *parent) : QObject(parent)
{
    m_buffer[INDEX_START_OF_FRAME] = FRAME_START;
    m_buffer[INDEX_CMD] = cmd;
    m_buffer[INDEX_DATA_LENGTH] = 1;
    m_buffer[INDEX_FIRST_DATA_BYTE] = quint8(data);
    m_buffer += CalculateChecksum();
}

Frame::Frame(quint8 cmd, quint16 data, QObject *parent) : QObject(parent)
{
    m_buffer[INDEX_START_OF_FRAME] = FRAME_START;
    m_buffer[INDEX_CMD] = cmd;
    m_buffer[INDEX_DATA_LENGTH] = 2;
    m_buffer[INDEX_FIRST_DATA_BYTE] = quint8((data & 0xFF00) >> 8);
    m_buffer[INDEX_FIRST_DATA_BYTE + 1] = quint8(data & 0x00FF);
    m_buffer += CalculateChecksum();
}

Frame::Frame(quint8 cmd, qint16 data, QObject *parent) : QObject(parent)
{
    m_buffer[INDEX_START_OF_FRAME] = FRAME_START;
    m_buffer[INDEX_CMD] = cmd;
    m_buffer[INDEX_DATA_LENGTH] = 2;
    m_buffer[INDEX_FIRST_DATA_BYTE] = quint8((data & 0xFF00) >> 8);
    m_buffer[INDEX_FIRST_DATA_BYTE + 1] = quint8(data & 0x00FF);
    m_buffer += CalculateChecksum();
}

Frame::Frame(quint8 cmd, quint32 data, QObject *parent) : QObject(parent)
{
    m_buffer[INDEX_START_OF_FRAME] = FRAME_START;
    m_buffer[INDEX_CMD] = cmd;
    m_buffer[INDEX_DATA_LENGTH] = 4;
    m_buffer[INDEX_FIRST_DATA_BYTE] = quint8((data & 0xFF000000) >> 24);
    m_buffer[INDEX_FIRST_DATA_BYTE + 1] = quint8((data & 0x00FF0000) >> 16);
    m_buffer[INDEX_FIRST_DATA_BYTE + 2] = quint8((data & 0x0000FF00) >> 8);
    m_buffer[INDEX_FIRST_DATA_BYTE + 3] = quint8(data & 0x000000FF);
    m_buffer += CalculateChecksum();
}

Frame::Frame(quint8 cmd, qint32 data, QObject *parent) : QObject(parent)
{
    m_buffer[INDEX_START_OF_FRAME] = FRAME_START;
    m_buffer[INDEX_CMD] = cmd;
    m_buffer[INDEX_DATA_LENGTH] = 4;
    m_buffer[INDEX_FIRST_DATA_BYTE] = quint8((quint32(data) & 0xFF000000) >> 24);
    m_buffer[INDEX_FIRST_DATA_BYTE + 1] = quint8((data & 0x00FF0000) >> 16);
    m_buffer[INDEX_FIRST_DATA_BYTE + 2] = quint8((data & 0x0000FF00) >> 8);
    m_buffer[INDEX_FIRST_DATA_BYTE + 3] = quint8(data & 0x000000FF);
    m_buffer += CalculateChecksum();
}

Frame::Frame(quint8 cmd, QByteArray data, QObject *parent) : QObject(parent)
{
   m_buffer[INDEX_START_OF_FRAME] = FRAME_START;

   m_buffer[INDEX_CMD] = cmd;
   m_buffer[INDEX_DATA_LENGTH] = quint8(data.count());

   for(int i = 0; i < data.count(); i++)
       m_buffer[INDEX_FIRST_DATA_BYTE + i] = quint8(data[i]);

   m_buffer += CalculateChecksum();
}


Frame::~Frame()
{
    m_buffer.clear();
}



quint8 Frame::GetCmd()
{
    quint8 rv = 0;
    if(m_buffer.count() > 3)
        rv = quint8(m_buffer[INDEX_CMD]);
    return rv;
}

quint8  Frame::GetDataLength()
{
    quint8 rv = 0;
    if(m_buffer.count() > INDEX_CMD)
        rv = quint8(m_buffer[INDEX_DATA_LENGTH]);
    return rv;
}

quint8 Frame::GetUByte()
{
    quint8 rv = 0;
    if(this->GetDataLength() > 0)
        rv = quint8(m_buffer[INDEX_FIRST_DATA_BYTE]);
    return rv;
}

quint8 Frame::GetUByte(int index)
{
    quint8 rv = 0;
    if(this->GetDataLength() > 0)
        rv = quint8(m_buffer[INDEX_FIRST_DATA_BYTE + index]);
    return rv;
}

qint8 Frame::GetSByte()
{
    qint8 rv = 0;
    if(this->GetDataLength() > 0)
        rv = qint8(m_buffer[INDEX_FIRST_DATA_BYTE]);
    return rv;
}

quint8 Frame::GetIndexedByte(int index)
{
    quint8 rv = 0;
    if(this->GetDataLength() > 0)
        rv = quint8(m_buffer[INDEX_FIRST_DATA_BYTE + index]);
    return rv;
}

quint16 Frame::GetUInt16()
{
    quint16 rv = 0;
    if(this->GetDataLength() > 0)
        rv = quint16(makeWord(m_buffer[INDEX_FIRST_DATA_BYTE + 1], m_buffer[INDEX_FIRST_DATA_BYTE]));
    return rv;
}

quint16 Frame::GetUInt16(int index)
{
    quint16 rv = 0;
    if(this->GetDataLength() > 0)
        rv = quint16(makeWord(m_buffer[INDEX_FIRST_DATA_BYTE + 1 + index], m_buffer[INDEX_FIRST_DATA_BYTE + index]));
    return rv;
}

qint16  Frame::GetInt16()
{
    qint16 rv = 0;
    if(this->GetDataLength() > 0)
        rv = qint16(makeWord(m_buffer[INDEX_FIRST_DATA_BYTE + 1], m_buffer[INDEX_FIRST_DATA_BYTE]));
    return rv;
}

quint32 Frame::GetUInt32()
{
    quint32 rv = 0;
    if(this->GetDataLength() > 0)
        rv = quint32(makeDWord(makeWord(m_buffer[INDEX_FIRST_DATA_BYTE + 3], m_buffer[INDEX_FIRST_DATA_BYTE + 2]),
                    makeWord(m_buffer[INDEX_FIRST_DATA_BYTE + 1], m_buffer[INDEX_FIRST_DATA_BYTE])));
    return rv;
}

quint32 Frame::GetUInt32(int index)
{
    quint32 rv = 0;
    if(this->GetDataLength() > 0)
        rv = quint32(makeDWord(makeWord(m_buffer[INDEX_FIRST_DATA_BYTE + 3 + index], m_buffer[INDEX_FIRST_DATA_BYTE + 2 + index]),
                    makeWord(m_buffer[INDEX_FIRST_DATA_BYTE + 1 + index], m_buffer[INDEX_FIRST_DATA_BYTE + index])));
    return rv;
}

qint32 Frame::GetInt32()
{
    qint32 rv = 0;
    if(this->GetDataLength() > 0)
        rv = qint32(makeDWord(makeWord(m_buffer[INDEX_FIRST_DATA_BYTE + 3], m_buffer[INDEX_FIRST_DATA_BYTE + 2]),
                    makeWord(m_buffer[INDEX_FIRST_DATA_BYTE + 1], m_buffer[INDEX_FIRST_DATA_BYTE])));
    return rv;
}

quint8  Frame::CalculateChecksum()
{
    quint8 rv = 0;
    for (int i = 0; i < m_buffer.count(); i++)
    {
        rv += quint8(m_buffer[i]);
    }
    return rv;
}

void Frame::Clear()
{
    m_buffer.clear();
}

void Frame::AddByte(quint8 data)
{
    m_buffer.append(data);
}

QByteArray Frame::GetBuffer()
{
    return m_buffer;
}

quint16 Frame::makeWord(quint8 l, quint8 h)
{
    quint16 rv = h;
    rv <<= 8;
    rv |= l;
    return rv;
}

quint32 Frame::makeDWord(quint16 l, quint16 h)
{
    quint32 rv = h;
    rv <<= 16;
    rv |= l;
    return rv;
}
