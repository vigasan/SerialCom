///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                             DEFINES
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define LENGTH_INPUT_BUFFER      50
#define LENGTH_OUT_DATA_BUFFER   50
#define LENGTH_OUT_FRAME_BUFFER  60

#define RCV_ST_IDLE              0
#define RCV_ST_CMD               1
#define RCV_ST_DATA_LENGTH       2
#define RCV_ST_DATA              3
#define RCV_ST_CHECKSUM          4

#define FRAME_START              0x8A
#define FRAME_ESCAPE_CHAR        0x8B
#define FRAME_XOR_CHAR           0x20
#define FRAME_NUM_EXTRA_BYTES    4 

#define INDEX_START_OF_FRAME     0
#define INDEX_CMD                1
#define INDEX_DATA_LENGTH        2
#define INDEX_FIRST_DATA_BYTE    3

#define BTN_UNPRESSED            0
#define BTN_DEBOUNCE             1
#define BTN_PRESSED              2

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Application Commands         ID       DIRECTION           DECRIPTION
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CMD_BUTTON_1             1    //  ESP32 -> RPI        BUTTON 1 STATUS (PRESSED, UNPRESSED)  
#define CMD_BUTTON_2             2    //  ESP32 -> RPI        BUTTON 2 STATUS (PRESSED, UNPRESSED)  
#define CMD_LED_GREEN            3      
#define CMD_PWM_LED_R            4    //  RPI -> ESP32        SET PWM DUTYCYCLE FOR RED LED (0 - 255) 
#define CMD_PWM_LED_G            5    //  RPI -> ESP32        SET PWM DUTYCYCLE FOR GREEN LED (0 - 255)
#define CMD_PWM_LED_B            6    //  RPI -> ESP32        SET PWM DUTYCYCLE FOR BLUE LED (0 - 255)
#define CMD_ADC_INPUT            7    //  ESP32 -> RPI        ADC READ VALUE (0 - 4095) 
#define CMD_ADC_ENABLE           8    //  RPI -> ESP32        ENABLE/DISABLE ADC READING/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                            I/O PIN DEFINITIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const int OUT_LED_RED = 5;
const int OUT_LED_GREEN = 16;
const int OUT_LED_BLUE = 17;

const int IN_BUTTON1 = 0;
const int IN_BUTTON2 = 4;
const int ADC_PIN = 35;

// setting PWM properties
const int freq = 5000;
const int resolution = 8;

const int ledChRed = 0;
const int ledChGreen = 1;
const int ledChBlue = 2;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                            Global Variables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte g_InputBuffer[LENGTH_INPUT_BUFFER];            // Incoming data buffer
byte g_OutFrameBuffer[LENGTH_OUT_FRAME_BUFFER];     // Data buffer for output Frame
byte g_OutBuffer[LENGTH_OUT_DATA_BUFFER];           // Data buffer for output data

byte g_ReceiverStatus = RCV_ST_IDLE;
byte g_xorValue = 0x00;
byte g_Checksum = 0;
int  g_DataLength = 0;
int  g_DataLengthFixed = 0;
int  g_BufferIndex = 0;

bool g_AdcEnabled = false;
int  g_AdcValue = 0;
int  g_st_Button1 = BTN_UNPRESSED;
int  g_st_Button2 = BTN_UNPRESSED;
unsigned long g_TimeStamp = 0;

void setup() 
{
    Serial.begin(115200);
    delay(100);
   
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // I/O Configuration
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    pinMode(OUT_LED_RED, OUTPUT);
    pinMode(OUT_LED_GREEN, OUTPUT);
    pinMode(OUT_LED_BLUE, OUTPUT);

    pinMode(IN_BUTTON1, INPUT);
    pinMode(IN_BUTTON2, INPUT);
    
    // Configure LED PWM functionalitites
    ledcSetup(ledChRed, freq, resolution);
    ledcSetup(ledChGreen, freq, resolution);
    ledcSetup(ledChBlue, freq, resolution);

    // Attach the channel to the GPIO to be controlled
    ledcAttachPin(OUT_LED_RED, ledChRed);
    ledcAttachPin(OUT_LED_GREEN, ledChGreen);
    ledcAttachPin(OUT_LED_BLUE, ledChBlue);
}

void loop() 
{
    if(Serial.available() > 0)
    {
        byte receivedByte = (byte)Serial.read();
        if(receivedByte == FRAME_ESCAPE_CHAR)
        {
            g_xorValue = FRAME_XOR_CHAR;
        } else
        {
            receivedByte ^= g_xorValue;
            g_xorValue = 0x00;

            switch(g_ReceiverStatus)
            {
                case RCV_ST_IDLE:
                {
                    if(receivedByte == FRAME_START)
                    {   
                        g_BufferIndex = 0;
                        g_InputBuffer[g_BufferIndex++] = receivedByte;
                        g_Checksum = receivedByte;
                        g_ReceiverStatus = RCV_ST_CMD;
                    }
                } break;

                case RCV_ST_CMD:
                {
                    g_InputBuffer[g_BufferIndex++] = receivedByte;
                    g_Checksum += receivedByte;
                    g_ReceiverStatus = RCV_ST_DATA_LENGTH;
                } break;
    
                case RCV_ST_DATA_LENGTH:
                {
                    g_DataLength = receivedByte;
                    g_DataLengthFixed = g_DataLength;
                    if(g_DataLength > 0)
                    {
                        g_InputBuffer[g_BufferIndex++] = receivedByte;
                        g_Checksum += receivedByte;
                        g_ReceiverStatus = RCV_ST_DATA;
                    } else
                    {   
                        g_ReceiverStatus = RCV_ST_IDLE;
                    }
                } break;
    
                case RCV_ST_DATA:
                {
                    g_InputBuffer[g_BufferIndex++] = receivedByte;
                    g_Checksum += receivedByte;
                    if(--g_DataLength == 0)
                        g_ReceiverStatus = RCV_ST_CHECKSUM;
                } break;
    
                case RCV_ST_CHECKSUM:
                {
                    if(receivedByte == g_Checksum && g_BufferIndex == (g_DataLengthFixed + FRAME_NUM_EXTRA_BYTES -1))
                    {   
                        g_ReceiverStatus = RCV_ST_IDLE;
                        g_InputBuffer[g_BufferIndex++] = receivedByte;

                        switch(g_InputBuffer[INDEX_CMD])
                        {
                            case CMD_PWM_LED_R:
                            {
                                ledcWrite(ledChRed, GetDataByte(g_InputBuffer));
                            } break;

                            case CMD_PWM_LED_G:
                            {
                                ledcWrite(ledChGreen, GetDataByte(g_InputBuffer));
                            } break;

                            case CMD_PWM_LED_B:
                            {
                                ledcWrite(ledChBlue, GetDataByte(g_InputBuffer));  
                            } break; 

                            case CMD_ADC_ENABLE:
                            {
                                g_AdcEnabled = (bool)GetDataByte(g_InputBuffer);
                            }
                        }
                    }
                } break;
            }
        }
    } else
    {
        //////////////////////////////////////////////////////////////////////////////////////////////
        /// Button 1 State Manager
        //////////////////////////////////////////////////////////////////////////////////////////////
        switch(g_st_Button1)
        {
            case BTN_UNPRESSED:
            {
                if(digitalRead(IN_BUTTON1) == 0)           // Button pressed
                {
                    g_st_Button1 = BTN_DEBOUNCE;
                } 
            } break;
        
            case BTN_DEBOUNCE:
            {
                if(digitalRead(IN_BUTTON1) == 0)           // Button is still pressed
                {
                    g_st_Button1 = BTN_PRESSED;
                    SendFrameByte(CMD_BUTTON_1, 1);
                } else                                     // Button is released
                {
                    g_st_Button1 = BTN_UNPRESSED;
                    SendFrameByte(CMD_BUTTON_1, 0);
                }
            } break;
        
            case BTN_PRESSED:
            {
                if(digitalRead(IN_BUTTON1) == 1)           // Button released
                {
                    g_st_Button1 = BTN_DEBOUNCE;
                }
            } break;
        } 

        //////////////////////////////////////////////////////////////////////////////////////////////
        /// Button 2 State Manager
        //////////////////////////////////////////////////////////////////////////////////////////////
        switch(g_st_Button2)
        {
            case BTN_UNPRESSED:
            {
                if(digitalRead(IN_BUTTON2) == 0)           // Button pressed
                {
                    g_st_Button2 = BTN_DEBOUNCE;
                } 
            } break;
        
            case BTN_DEBOUNCE:
            {
                if(digitalRead(IN_BUTTON2) == 0)           // Button is still pressed
                {
                    g_st_Button2 = BTN_PRESSED;
                    SendFrameByte(CMD_BUTTON_2, 1);
                } else                                     // Button is released
                {
                    g_st_Button2 = BTN_UNPRESSED;
                    SendFrameByte(CMD_BUTTON_2, 0);
                }
            } break;
        
            case BTN_PRESSED:
            {
                if(digitalRead(IN_BUTTON2) == 1)           // Button released
                {
                    g_st_Button2 = BTN_DEBOUNCE;
                }
            } break;
        }

        //////////////////////////////////////////////////////////////////////////////////////////////
        /// Adc Manager
        //////////////////////////////////////////////////////////////////////////////////////////////
        if(millis() > g_TimeStamp + 10)  // Every 10 ms
        {
            g_TimeStamp = millis();
            g_AdcValue = analogRead(ADC_PIN);
            if(g_AdcEnabled == true)
                SendFrameWord(CMD_ADC_INPUT, g_AdcValue);
        } 
    }

}

/*************************************************************************************************************************************************/
//NAME:         GetDataByte
//DESCRPTION:   Get byte data in the frame
//INPUT:        (byte*) Frame Pointer
//RETURN:       (byte) data frame
//NOTE:         
/*************************************************************************************************************************************************/
byte GetDataByte(byte* frame)
{
    byte rv = 0;
    if(frame)
    {
        rv = frame[INDEX_FIRST_DATA_BYTE];
    }
    return rv;
}

/*************************************************************************************************************************************************/
//NAME:         GetDataWord
//DESCRPTION:   Get word data (2 bytes) in the frame
//INPUT:        (byte*) Frame Pointer
//RETURN:       (word) data frame
//NOTE:         
/*************************************************************************************************************************************************/
word GetDataWord(byte* frame)
{
    word rv = 0;
    if(frame)
        rv = ((word)frame[INDEX_FIRST_DATA_BYTE] << 8) | (word)frame[INDEX_FIRST_DATA_BYTE + 1];
    return rv;
}

/*************************************************************************************************************************************************/
//NAME:         GetDataLong
//DESCRPTION:   Get long data (4 bytes) in the frame
//INPUT:        (byte*) Frame Pointer
//RETURN:       (unsigned long) data frame
//NOTE:         
/*************************************************************************************************************************************************/
unsigned long GetDataLong(byte* frame)
{
    unsigned long rv = 0;
    word hiWord = 0, loWord = 0;
    if(frame)
    {
        hiWord = ((word)frame[INDEX_FIRST_DATA_BYTE] << 8) | (word)frame[INDEX_FIRST_DATA_BYTE + 1];
        loWord = ((word)frame[INDEX_FIRST_DATA_BYTE + 2] << 8) | (word)frame[INDEX_FIRST_DATA_BYTE + 3];
        rv = (((unsigned long)(hiWord)) << 16) | (unsigned long)((unsigned long)(loWord)) ;
    }
    return rv;
}

/*************************************************************************************************************************************************/
//NAME:         GetDataPointer
//DESCRPTION:   Get pointer to the first data byte in the frame
//INPUT:        (byte*) Frame Pointer
//RETURN:       (byte*) Pointer to the first data byte in the frame
//NOTE:         
/*************************************************************************************************************************************************/
byte* GetDataPointer(byte* frame)
{
    return frame + INDEX_FIRST_DATA_BYTE;
}

/*************************************************************************************************************************************************/
//NAME:         SendFrameByte
//DESCRPTION:   Send a byte value
//INPUT:        (byte) command id
//              (byte) data to send
//RETURN:       void
//NOTE:
/*************************************************************************************************************************************************/
void SendFrameByte(byte cmd, byte data)
{
    g_OutFrameBuffer[0] = FRAME_START;                         // Start Frame
    g_OutFrameBuffer[1] = cmd;                                     // Comando
    g_OutFrameBuffer[2] = 0x01;                                    // Lunghezza campo dati
    g_OutFrameBuffer[3] = data;                                    // Data
    g_OutFrameBuffer[4] = CalculateChecksum(4);                    // Checksum

    SendFrame(g_OutFrameBuffer, 5);
}

/*************************************************************************************************************************************************/
//NAME:         SendFrameWord
//DESCRPTION:   Send a word value(2 bytes)
//INPUT:        (byte) command id
//              (word) data to send
//RETURN:       void
//NOTE:
/*************************************************************************************************************************************************/
void SendFrameWord(byte cmd, word data)
{
    g_OutFrameBuffer[0] = FRAME_START;                             // Start Frame
    g_OutFrameBuffer[1] = cmd;                                     // Comando
    g_OutFrameBuffer[2] = 0x02;                                    // Lunghezza campo dati
    g_OutFrameBuffer[3] = (data & 0xFF00) >> 8;                    
    g_OutFrameBuffer[4] = data & 0x00FF;
    g_OutFrameBuffer[5] = CalculateChecksum(5);                    // Checksum

    SendFrame(g_OutFrameBuffer, 6);
}

/*************************************************************************************************************************************************/
//NAME:         SendFrameLong
//DESCRPTION:   Send a long value(4 bytes)
//INPUT:        (byte) command id
//              (unsigned long) data to send
//RETURN:       void
//NOTE:
/*************************************************************************************************************************************************/
void SendFrameLong(byte cmd, unsigned long data)
{
    g_OutFrameBuffer[0] = FRAME_START;                              // Start Frame
    g_OutFrameBuffer[1] = cmd;                                      // Comando
    g_OutFrameBuffer[2] = 0x04;                                     // Lunghezza campo dati
    g_OutFrameBuffer[3] = (data & 0xFF000000) >> 24;                // Data
    g_OutFrameBuffer[4] = (data & 0x00FF0000) >> 16;
    g_OutFrameBuffer[5] = (data & 0x0000FF00) >> 8;
    g_OutFrameBuffer[6] = data & 0x000000FF;
    g_OutFrameBuffer[7] = CalculateChecksum(7);                     // Checksum

    SendFrame(g_OutFrameBuffer, 8);
}

/*************************************************************************************************************************************************/
//NAME:         SendFrameBuffer
//DESCRPTION:   Send a data buffer
//INPUT:        (byte) command id
//              (byte*) buffer pointer
//              (byte) buffer length
//RETURN:       void
//NOTE:
/*************************************************************************************************************************************************/
void SendFrameBuffer(byte cmd, byte* pBuff, int length)
{
    int i = 0;
    g_OutFrameBuffer[0] = FRAME_START;                             // Start Frame
    g_OutFrameBuffer[1] = cmd;                                     // Comando
    g_OutFrameBuffer[2] = length;                                  // Lunghezza campo dati
    for(i = 0; i < length; i++)
        g_OutFrameBuffer[i + 3] = pBuff[i];
    g_OutFrameBuffer[length + 3] = CalculateChecksum(length + 3);  // Checksum

    SendFrame(g_OutFrameBuffer, length + 4);
}

/*************************************************************************************************************************************************/
//NAME:         CalculateChecksum
//DESCRPTION:   Calculate Checksum value 
//INPUT:        (byte) Frame buffer length
//RETURN:       (byte) Checksum value
//NOTE:
/*************************************************************************************************************************************************/
byte CalculateChecksum(byte length)
{
    byte rv = 0, index;
    for(index = 0; index < length; index++)
    {
        rv += g_OutFrameBuffer[index];
    }
    return rv;
}

/*************************************************************************************************************************************************/
//NAME:         SendFrame
//DESCRPTION:   Send Frame over serial port
//INPUT:        (byte*) Frame buffer pointer
//              (byte) length
//RETURN:       void
//NOTE:
/*************************************************************************************************************************************************/
void SendFrame(byte* pFrameBuff, int length)
{
    int i;
    byte dataToSend = 0;

    g_OutBuffer[dataToSend++] = FRAME_START;

    for(i = 1; i < length; i++)
    {
        if(pFrameBuff[i] == FRAME_START || pFrameBuff[i] == FRAME_ESCAPE_CHAR)
        {
            g_OutBuffer[dataToSend++] = FRAME_ESCAPE_CHAR;
            g_OutBuffer[dataToSend++] = pFrameBuff[i] ^ FRAME_XOR_CHAR;
        } else
            g_OutBuffer[dataToSend++] = pFrameBuff[i];
    }
    
    Serial.write(g_OutBuffer, dataToSend);
}
