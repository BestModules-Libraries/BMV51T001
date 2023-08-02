/******************************************************************
File:             PlayMidi.ino
Description:      BMduino sends MIDI data to the BMV51T001 for MIDI playback
Note:             -       
******************************************************************/



#include "BMV51T001.h"
#include "playMIDI.h"
#include "heiPanPo.h"


playMIDI myMIDIPlayer;

void setup() {
    myMIDIPlayer.begin();

    myMIDIPlayer.setChannelVolume(127, 0 + 1); //Set channel 1 volume to 127 (loudest)
    myMIDIPlayer.setTone(35, 1 + 1);  //Set channel 2 tone to 35 (Fretless Bass)
    myMIDIPlayer.setChannelVolume(127, 1 + 1); //Set channel 2 volume to 127 (loudest)
    myMIDIPlayer.setChannelVolume(127, 9 + 1); //Set channel 10 volume to 127 (loudest)
    // Note: In Midi files, channels start at 0 and functions start at 1, so +1 is required
    //Starts playing the MIDI song corresponding to the MUSIC array element
    myMIDIPlayer.beginPlayMIDI(music,TEMPO,DIVISION,NOTENUM);
}

void loop() {
    myMIDIPlayer.loopPlayMIDI();//Perform MIDI song playback
    if(myMIDIPlayer.isPlaying() == MIDI_PLAYER_NO_BUSY)//Is in the play?
    {
       myMIDIPlayer.beginPlayMIDI(music,TEMPO,DIVISION,NOTENUM); //Repeats play
    } 
}
