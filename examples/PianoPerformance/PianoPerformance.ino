/******************************************************************
File:             PianoPerformance.ino
Description:      Matrix keys simulate a keyboard
                  Pressing the keys trigger the MIDI Shield sound
Note:                    
******************************************************************/
#include "BMV51T001.h"

BMV51T001 shieldMIDI;  //Create an object
uint8_t keyBuf[8];  //Store the key data. The size must be 8, because each reception is 8 bytes
uint8_t lastKeyBuf[8];//Storing key data
uint8_t temp,i,j;
uint8_t noteNumber = 0;
uint8_t velocity = 0x50;//Intensity, the piano did not do the intensity detection, default 0x50
uint8_t channel = 0x01;//Note plays on Channel 1 by default
uint8_t keyOutEnableFlag = 0;

void setup() 
{
    shieldMIDI.begin(); //Object initialization
    if(shieldMIDI.setKeyboardOut(BMV51T001_KEYBOARD_ENABLE))//The keyboard function was enabled
    {
        keyOutEnableFlag = 1;
    }   
}

void loop() 
{
    if(keyOutEnableFlag)
    {
        shieldMIDI.scanKeyboard();
        if(shieldMIDI.isKeyboard() != 0)//there are key changes
        {   
            shieldMIDI.readKeyboardData(keyBuf);
            
            for (i = 0; i < 8; i++)    //Judge all 8 groups of keys once
            {
                temp = keyBuf[i]^lastKeyBuf[i];
                if (temp!=0)  //If there is a key to press
                {
                    for (j = 0; j < 8; j++)//There are eight keys in each group of keys
                    {
                        if ((temp &(1 << j))!=0)//This position has key change
                        {
                            //Since it is 64 key, start with the DO of noteNumber 36 big letters
                            noteNumber = 36 +i*8+j ;
                            if ((keyBuf[i]&(1 << j)) !=0)  //key press
                            {
                                //Call Note On to play the note
                                shieldMIDI.setNoteOn (noteNumber, velocity, channel);
                            }
                            else
                            {
                                //Call Note off to stop the note
                                shieldMIDI.setNoteOff (noteNumber, velocity, channel);
                            }
                        }
                    }
                    lastKeyBuf[i]= keyBuf [i];
                }
            }
        }      
    }
    
}
