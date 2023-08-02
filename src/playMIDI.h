/***************************************************************************
File:       	  palyMIDI.h
Author:           BESTMODULE
Description:      Play .mid file
History：		  -
	V1.0.1	 -- initial version；2022-07-29；Arduino IDE : v1.8.13

****************************************************************************/
#ifndef  __PLAY_MIDI_H
#define __PLAY_MIDI_H

#include "BMV51T001.h"


#define MIDI_PLAYER_BUSY 1
#define MIDI_PLAYER_NO_BUSY 0


typedef struct {
	uint32_t time;
	uint32_t ctrlnum;
	uint8_t  value;
	uint8_t  channel;
} midi_struct;


class playMIDI:public BMV51T001{
public:
    playMIDI();
    void beginPlayMIDI(midi_struct MidData[],uint32_t tempo,uint16_t division,uint16_t noteSum);
    void endPlayMIDI(void);
    void loopPlayMIDI(void); 
    bool isPlaying(void);
private:
    midi_struct *pMidData;//Mid data
    uint32_t _tempo;//Duration of each beat (unit: us)
    uint16_t _division;//Tick number per beat
    uint16_t _noteSum;//The total number of notes in a MIDI song
    uint8_t _enableFlag;
    uint32_t _startMillis;
    uint32_t _index;//the address offset in the MIDI data array
};

#endif




