/*************************************************************************
File:       	    BMV51T001.cpp
Author:           BESTMODULES
Description:      The UART communicates with the BMV51T001
History：		  -
	V1.0.1	 -- initial version；2023-01-17；Arduino IDE : v1.8.13

**************************************************************************/

#include "BMV51T001.h"

#define GET_FW_VERSION              (0X01)
#define RESET_CMD                   (0x02)
#define VOLUME_CMD                  (0x03)
#define HIT_SENSITIVITY_CMD         (0X04)
#define HIT_TIME_INTERVAL_CMD       (0X05)
#define SET_KEYBOARD_OUT	        (0X06)
#define	SET_HIT_OUT	                (0X07)

#define     USE_RUNNING_STATUS      (true)
#define     NOT_USE_RUNNING_STATUS  (false)

#define     MIDI_CHANNEL_OMNI       (0)
#define     MIDI_CHANNEL_OFF        (17) // and over

#define     MIDI_PITCHBEND_MIN      (-8192)
#define     MIDI_PITCHBEND_MAX      (8191)


#define ENABLE_MIDI_PROTOCOL digitalWrite(7, HIGH)
#define ENABLE_CMD_PROTOCOL digitalWrite(7, LOW)
#define BMV51T001_OPT_SUCCESS 1
#define BMV51T001_OPT_FAILED 0
#define BMV51T001_ACK_SUCCESS 0x3e
#define BMV51T001_ACK_FAILED 0xe3
/************************************************************************* 
Description:    Constructor
parameter:
    Input:          
    Output:         
Return:         
Others:         
*************************************************************************/
BMV51T001::BMV51T001()
{
    _hitADCData = 0;
    _mRunningStatus = false;
    _mRunningStatus_TX = 0x00;
    _isKeyFlag = 0;
    _isHitFlag = 0;
    _strengthLayers = 0;
    _strength = 0;
    _firstData = 0;
    _firstIn = 0;
    memset(_hitBuf, 0, 2);
    memset(_keyBuf, 0, 8);
}
/************************************************************************* 
Description:    MIDI communication initialization
parameter:
    Input:          
    Output:         
Return:         
Others:         
*************************************************************************/
void BMV51T001::begin(void)
{
    //Flag pin initialization
    pinMode(6, INPUT_PULLUP);
    pinMode(7, OUTPUT);
    pinMode(8, INPUT_PULLUP);
    pinMode(9, INPUT_PULLUP);
    digitalWrite(7, HIGH);

    
    Serial.begin(31250);//Initialize the baud rate of the serial port（31250：MIDI communication baud rate）
    delay(100);
    reset();//Reset the slave machine's parameters each time you re - download

}
/************************************************************************* 
Description:    Write bytes
parameter:
    Input:      wbuf[] : the datas that need to be sent
                wlen :  the length of datas  
    Output: 
Return:         Read status  1:Success 0:Fail     
Others:         
**************************************************************************/
void BMV51T001::writeBytes(uint8_t wbuf[], uint8_t wlen)
{
    uint8_t i;
	for (i = 0; i < wlen; i++)
	{		
		Serial.write(wbuf[i]); 
	}
}
/************************************************************************* 
Description:    Read bytes
parameter:
    Input:      wlen :  the length of datas need to read 
    Output:     rbuf[] : Store the read back datas
Return:         Read status  1:Success 0:Fail     
Others:         
**************************************************************************/
bool BMV51T001::readBytes(uint8_t rbuf[], uint8_t wlen)
{
    uint8_t cnt = 0;
    uint8_t delay_count = 0;
    while(1)
    {

        if (Serial.available() > 0)
        {
            if(cnt < wlen)
            {
                rbuf[cnt++] = Serial.read();
                if(cnt==wlen)
                     return BMV51T001_ACK_SUCCESS;
            }
        }
        delay_count++;
        delayMicroseconds(50);//waiting for receive data 
        if(delay_count>=200)return BMV51T001_OPT_FAILED;//timeout is 50us*200=10ms,nothing for receive
    }
}

/************************************************************************* 
Description:    Set the BMV51T001 volume
parameter:
    Input:    volume: 0~15(0: Minimum volume(mute))      
    Output:        
Return:         Read status  1:Success 0:Fail   
Others:         
*************************************************************************/
bool BMV51T001::setVolume(uint8_t volume)
{
    uint8_t result=BMV51T001_OPT_FAILED;
    uint8_t rwbuf[4]={0XAF,VOLUME_CMD};

    ENABLE_CMD_PROTOCOL;
    rwbuf[2]=volume;
    rwbuf[3]=volume+VOLUME_CMD;//check sum
    writeBytes(rwbuf,4);
    if(readBytes(rwbuf,1)==BMV51T001_OPT_SUCCESS)
    {  
         if(rwbuf[0]==BMV51T001_ACK_SUCCESS)
            result=BMV51T001_OPT_SUCCESS;
    }
    return result;
}
/************************************************************************* 
Description:    Setting hit sensitivity,It's the threshold to detect a hit
parameter:
    Input:          value:Level of sensitivity(0~18)
                    The higher the value, the higher the threshold and 
                    the lower the sensitivity
                    value = 0 --> threshold = 100 + 50 * value = 100
                    .
                    .
                    .
                    value = 18 --> threshold = 100 + 50 * value = 1000
                    The threshold initial value is 100
    Output:         
Return:         Read status  1:Success 0:Fail   
Others:         
*************************************************************************/
bool BMV51T001::setHitSensitivity(uint8_t value)
{
    uint8_t result=BMV51T001_OPT_FAILED;
    uint8_t rwbuf[4]={0XAF,HIT_SENSITIVITY_CMD};

    ENABLE_CMD_PROTOCOL;
    rwbuf[2]=value;
    rwbuf[3]=value+HIT_SENSITIVITY_CMD;//check sum
    writeBytes(rwbuf,4);
    if(readBytes(rwbuf,1)==BMV51T001_OPT_SUCCESS)
    {  
         if(rwbuf[0]==BMV51T001_ACK_SUCCESS)
            result=BMV51T001_OPT_SUCCESS;
    }
    return result;
}
/************************************************************************* 
Description:    Setting the time interval between two hits
parameter:
    Input:          value:Level of time interval(0~33)
                    The higher the value, the higher the time interval
                    value = 0 --> time interval = 35 + 5 * value = 35ms
                    .
                    .
                    .
                    value = 33 --> time interval = 35 + 5 * value = 200ms
                    The time interval initial value is 50
    Output:         
Return:         Read status  1:Success 0:Fail   
Others:         
*************************************************************************/
bool BMV51T001::setHitTimeInterval(uint8_t value)
{
    uint8_t result=BMV51T001_OPT_FAILED;
    uint8_t rwbuf[4]={0XAF,HIT_TIME_INTERVAL_CMD};

    ENABLE_CMD_PROTOCOL;
    rwbuf[2]=value;
    rwbuf[3]=value+HIT_TIME_INTERVAL_CMD;//check sum
    writeBytes(rwbuf,4);
    if(readBytes(rwbuf,1)==BMV51T001_OPT_SUCCESS)
    {  
         if(rwbuf[0]==BMV51T001_ACK_SUCCESS)
            result=BMV51T001_OPT_SUCCESS;
    }
    return result;
}
/************************************************************************* 
Description:    Set the hit strength layers
parameter:
    Input:          layers：The layer number of strength(1~128)
    Output:         
Return:         
Others:         
**************************************************************************/
void BMV51T001::setHitStrengthLayer(uint8_t layers)
{
    _strengthLayers = layers;
} 
/************************************************************************* 
Description:    Enable 8x8key keyboard output
parameter:
    Input:          status：1: enable; 0: disable
    Output:         
Return:         Read status  1:Success 0:Fail   
Others:         
*************************************************************************/
bool BMV51T001::setKeyboardOut(uint8_t status)
{
    uint8_t result=BMV51T001_OPT_FAILED;
    uint8_t rwbuf[4]={0XAF,SET_KEYBOARD_OUT};

    ENABLE_CMD_PROTOCOL;
    rwbuf[2]=status;
    rwbuf[3]=status+SET_KEYBOARD_OUT;//check sum
    writeBytes(rwbuf,4);
    if(readBytes(rwbuf,1)==BMV51T001_OPT_SUCCESS)
    {  
         if(rwbuf[0]==BMV51T001_ACK_SUCCESS)
            result=BMV51T001_OPT_SUCCESS;
    }
    return result;
}
/************************************************************************* 
Description:    Enable hit output
parameter:
    Input:          status：1: enable; 0: disable
    Output:         
Return:         Read status  1:Success 0:Fail 
Others:         
*************************************************************************/
bool BMV51T001::setHitOut(uint8_t status)
{
    uint8_t result=BMV51T001_OPT_FAILED;
    uint8_t rwbuf[4]={0XAF,SET_HIT_OUT};

    ENABLE_CMD_PROTOCOL;
    rwbuf[2]=status;
    rwbuf[3]=status+SET_HIT_OUT;//check sum
    writeBytes(rwbuf,4);
    if(readBytes(rwbuf,1)==BMV51T001_OPT_SUCCESS)
    {  
         if(rwbuf[0]==BMV51T001_ACK_SUCCESS)
            result=BMV51T001_OPT_SUCCESS;
    }
    return result;
}
/************************************************************************* 
Description:    Scan 8x8key 
parameter:
    Input:          
    Output:         
Return:         
Others:         
*************************************************************************/
void BMV51T001::scanKeyboard(void)
{
    static uint8_t keyNum = 0;
    if(0 == _firstIn)
    {
        _isKeyFlag = 0;
        if (Serial.available() > 0)
        {
            _firstIn = 1;
            _firstData = Serial.read();  
        }
    }
    if (0x55 == _firstData)
    {
        if (keyNum < 8)
        {                
            if (Serial.available() > 0)
            {
                _keyBuf[keyNum++] = Serial.read();
            }
        }
        if (8 == keyNum)
        {
            _firstIn = 0;
            _firstData = 0;
            _isKeyFlag = 1;
            keyNum = 0;
        }
    }     
}
/************************************************************************* 
Description:    Scan hit
parameter:
    Input:          
    Output:         
Return:         
Others:         
*************************************************************************/
void BMV51T001::scanHit(void)
{
    static uint8_t k = 0;
    if(0 == _firstIn)
    {
        _isHitFlag = 0;
        if (Serial.available() > 0)
        {
            _firstIn = 1;
            _firstData = Serial.read();  
        }
    }
    if (0xAA == _firstData)
    {
        if (k < 2)  
        {
            if (Serial.available() > 0)
            {
                _hitBuf[k++] = Serial.read();
            }
        }
        if (2 == k)//Receive complete
        {
            _firstIn = 0;
            _firstData = 0;
            _isHitFlag = 1;
            k = 0;
        }
        _hitADCData = ((uint16_t)_hitBuf[1] << 8) + _hitBuf[0];  
    }   
    
}
/************************************************************************* 
Description:    There are key changes
parameter:
    Input:          
    Output:         
Return:         1: There are key changes; 0: No key changes
Others:         
*************************************************************************/
bool BMV51T001::isKeyboard(void)
{
    return _isKeyFlag;
}
/************************************************************************* 
Description:    A hit is detected
parameter:
    Input:          
    Output:         
Return:         1: A hit; 0: There is no hit
Others:         
*************************************************************************/
bool BMV51T001::isHit(void)
{
    return _isHitFlag;
}
/************************************************************************* 
Description:    Read Keyboard data into the keyBuf array.
parameter:
    Input:          
    Output:         
Return:         
Others:         
**************************************************************************/
void BMV51T001::readKeyboardData(uint8_t keyBuf[])
{
    memcpy(keyBuf, _keyBuf, 8);
}
/************************************************************************* 
Description:    Read hit ADC data
parameter:
    Input:          
    Output:         
Return:         Returns hit ADC data
Others:         
**************************************************************************/
uint16_t BMV51T001::readHitADCData(void)
{
    return _hitADCData;
}
/************************************************************************* 
Description:    Gets the strength layer corresponding to the current hit data.
parameter:
    Input:      hitData：hit data
    Output:         
Return:         Returns hit strength
Others:         
**************************************************************************/
uint8_t BMV51T001::getHitStrengthLayer(uint16_t hitADCData)
{
    _strength = (uint8_t)(hitADCData/(4096/_strengthLayers));
    return(_strength);
} 
/************************************************************************* 
Description:    reset BMV51T001
parameter:
    Input:          
    Output:         
Return:         Read status  1:Success 0:Fail   
Others:         
*************************************************************************/
bool BMV51T001::reset(void)
{
    uint8_t result=BMV51T001_OPT_FAILED;
    uint8_t rwbuf[3]={0XAF,RESET_CMD,RESET_CMD};

    ENABLE_CMD_PROTOCOL;
    writeBytes(rwbuf,3);
    if(readBytes(rwbuf,1)==BMV51T001_OPT_SUCCESS)
    {  
         if(rwbuf[0]==BMV51T001_ACK_SUCCESS)
            result=BMV51T001_OPT_SUCCESS;
    }
    return result;
}
/************************************************************************* 
Description:    Get F/W version with BMV51T001
parameter:
    Input:          
    Output:         
Return:         0：Failed to get the version value. non-zero：version value
Others:         
*************************************************************************/
uint8_t BMV51T001::getFWVer(void)
{
    uint8_t result=BMV51T001_OPT_FAILED;
    uint8_t rwbuf[3]={0XAF,GET_FW_VERSION,GET_FW_VERSION};

    ENABLE_CMD_PROTOCOL;
    writeBytes(rwbuf,3);
    if(readBytes(rwbuf,2)==BMV51T001_OPT_SUCCESS)
    {  
        if(rwbuf[0]==BMV51T001_ACK_SUCCESS)
            result = rwbuf[1];
    }
    return result;
}

/************************************************************************* 
Description:    Sends MIDI channel messages, drum tone in channel 10
parameter:
    Input:      type：   MIDI status bytes (excluding channels)
                        fit parameters are NoteOff/NoteOn/ControlChange/ProgramChange/PitchBend
                data1：  MIDI Message data byte 1(0~127)
                data2：  MIDI Message data byte 2(0~127)
                channel：MIDI channel(1~16)
    Output:         
Return:         
Others:         when use channel 10, the data1 ranges from 24 to 84
**************************************************************************/
void BMV51T001::sendMIDI(MidiType type, uint8_t data1, uint8_t data2, uint8_t channel)
{
     ENABLE_MIDI_PROTOCOL;

    if (type <= PitchBend)// Channel messages
    {
        if (channel >= MIDI_CHANNEL_OFF || channel == MIDI_CHANNEL_OMNI || type < 0x80)
        {
            return;//Exit if the channel is wrong or the message is not of MIDI type
        }
        data1 &= 0x7f;//The value ranges from 0x00 to 0x7f to prevent users from sending incorrect data
        data2 &= 0x7f;
        
        uint8_t status = uint8_t(type | ((channel-1) & 0x0f));//(type, channel);
        //if (isMidiType(type))

        {
            if (_mRunningStatus)
            {
                if (_mRunningStatus_TX != status)//Note The status bytes have changed
                {
                  _mRunningStatus_TX = status;//Store the new status bytes for the next comparison
                  Serial.write(_mRunningStatus_TX);
                }		
            }
            else//Do not use the run state
            {
                Serial.write(status);//No running state is used, so the status bytes are sent regardless of whether they change
            }
            
            /*Sending data part*/
            Serial.write(data1);
            if (type != ProgramChange)//Except for these two state bytes,  all other channel messages contain two data bytes
            {
                Serial.write(data2);
            }
            delayMicroseconds(1000);//Wait for the slave to finish receiving
        }
    }
}
/************************************************************************* 
Description:    Send note on MIDI message, drum tone in channel 10
parameter:
    Input:      noteNumber：note(0~127)
                velocity：0~127
                channel：MIDI channel(1~16)
    Output:         
Return:         
Others:         when use channel 10, the noteNumber ranges from 24 to 84
**************************************************************************/
void BMV51T001::setNoteOn(uint8_t noteNumber, uint8_t velocity, uint8_t channel)
{
    sendMIDI(NoteOn, noteNumber, velocity, channel);
}


/************************************************************************* 
Description:    Send note off MIDI message, drum tone in channel 10
parameter:
    Input:      noteNumber：note(0~127)
                velocity：0~127
                channel：MIDI channel(1~16)
    Output:         
Return:         
Others:         when use channel 10, the noteNumber ranges from 24 to 84
**************************************************************************/
void BMV51T001::setNoteOff(uint8_t noteNumber, uint8_t velocity, uint8_t channel)
{
    sendMIDI(NoteOff, noteNumber, velocity, channel);
}
/************************************************************************* 
Description:    Send Program Change MIDI message
parameter:
    Input:          toneNumber：program number(0~127)
                    channel：MIDI channel(1~16)
    Output:         
Return:         
Others:         
**************************************************************************/
void BMV51T001::setTone(uint8_t toneNumber, uint8_t channel)
{
    sendMIDI(ProgramChange, toneNumber, 0, channel);
}
/************************************************************************* 
Description:    Set the channel volume level.
parameter:
    Input:      volume：0~127(0:minimum channel volume, 127:maximum channel volume)
                channel：MIDI channel(1~16)
    Output:         
Return:         
Others:         Note that it is different from the volume in the onboard buttons
**************************************************************************/
void BMV51T001::setChannelVolume(uint8_t channelVolume, uint8_t channel)
{
    sendMIDI(ControlChange, ChannelVolume, channelVolume, channel);
}
/************************************************************************* 
Description:    Send a Pitch Bend message using a signed integer value.
parameter:
    Input:      pitchValue：The amount of bend to send (in a signed integer format),
                            The range is -8192 ~ 8191
                            0 : the central (no bend) setting.
                            -8192 :the maximum downwards bend, and
                            8191 :the maximum upwards bend.
                channel：The channel on which the message will be sent(1~16)
    Output:         
Return:         
Others:         
**************************************************************************/
void BMV51T001::setPitchBend(int16_t pitchValue, uint8_t channel)
{
    const unsigned bend = unsigned(pitchValue - int16_t(MIDI_PITCHBEND_MIN));
    sendMIDI(PitchBend, (bend & 0x7f), (bend >> 7) & 0x7f, channel);		    
}

