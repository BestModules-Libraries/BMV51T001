/*************************************************************************
File:       	    palyMIDI.cpp
Author:           BESTMODULE
Description:        Play .mid file
History：		  -
	V1.0.1	 -- initial version；2022-07-29；Arduino IDE : v1.8.13

**************************************************************************/
#include "playMIDI.h"

#define MIDI_PLAYER_ENABLE 1
#define MIDI_PLAYER_DISABLE 0


/************************************************************************* 
Description:    Constructor
Input:          
Output:         
Return:         
Others:         
*************************************************************************/
playMIDI::playMIDI()
{
    _tempo = 0;
    _division = 0;
    _noteSum = 0;
    _enableFlag = MIDI_PLAYER_DISABLE;
    _startMillis = 0;
    _index = 0;
}
/************************************************************************* 
Description:    Initialization before playing the MID song
Input:          MidData[]: An array of MIDI music data
                tempo: Duration of each beat (unit: us)
                division: Tick number per beat
                noteSum: The total number of notes in a MIDI song
Output:         
Return:         
Others:         
*************************************************************************/
void playMIDI::beginPlayMIDI(midi_struct MidData[], uint32_t tempo, uint16_t division, uint16_t noteSum)
{
    _startMillis = millis();//Mark the start time of a song
    _enableFlag = MIDI_PLAYER_ENABLE;
    _index = 0;
    _tempo = tempo;
    _division = division;
    _noteSum = noteSum;
    pMidData = MidData;
}
/************************************************************************* 
Description:    End MIDI music playing
Input:          
Output:         
Return:         
Others:         
*************************************************************************/
void playMIDI::endPlayMIDI(void)
{
    _enableFlag = MIDI_PLAYER_DISABLE;
}
/************************************************************************* 
Description:    Play a MIDI song
Input:          
Output:         
Return:         
Others:         
*************************************************************************/
void playMIDI::loopPlayMIDI(void)
{
    uint32_t calculation_time ;
    if(_enableFlag == MIDI_PLAYER_DISABLE)
    {
        return;
    }
    calculation_time = _tempo / _division ;//Duration of each beat (unit: us)Not on the air
    calculation_time *= pMidData[_index].time ;//Current note tick converted to time value (unit: us)
    calculation_time /= 1000;//The unit is converted to ms
    if(millis() - _startMillis > calculation_time)//Determines whether the current note has timed out
    {
        //Send play information about the next note, Ctrlnum is pitch, value is intensity
        setNoteOn(pMidData[_index].ctrlnum, pMidData[_index].value, pMidData[_index].channel + 1); //Channel +1 because MDI files have channels that start at 0, whereas library functions start at 1
        _index++;//Play the next note
        if (_index >= _noteSum ) //If all notes of the current MIDI song are sent
        {
            _enableFlag = MIDI_PLAYER_DISABLE;//End of MIDI song
        }     
    }
}
/************************************************************************* 
Description:    Whether the MIDI song is still playing
Input:          
Output:         
Return:         1: In the play; 0: Not in the play
Others:         
*************************************************************************/
bool playMIDI::isPlaying(void)
{
    return _enableFlag;
}










