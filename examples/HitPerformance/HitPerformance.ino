/******************************************************************
File:             HitPerformance.ino
Description:      Hitping a piezoelectric plate triggers a drum tone
Note:                    
******************************************************************/
#include "BMV51T001.h"
BMV51T001 shieldMIDI;  //Create an object
uint16_t hitADCData = 0; //Store the hit data you read
uint8_t velocity = 0;
uint8_t noteNumber = 0x40;//Define the drums tone
uint8_t channel = 10;//Drum sound fixed in channel 10
uint8_t hitOutEnableFlag = 0;
#define BMV51T001_NO_HIT 0

void setup() 
{
    shieldMIDI.begin(); //Object initialization
    shieldMIDI.setHitStrengthLayer(128); //Set the number of intensity layers, the total intensity is divided into 128 layers
    if(shieldMIDI.setHitOut(BMV51T001_HIT_ENABLE))
    {
        hitOutEnableFlag = 1;
    }   
}

void loop() 
{
    if(hitOutEnableFlag)
    {
        shieldMIDI.scanHit();
        if(shieldMIDI.isHit() != BMV51T001_NO_HIT)//There are hiting
        {
            //Read hit detection data
            hitADCData = shieldMIDI.readHitADCData();
            //Get the intensity level corresponding to the current hit data
            velocity = shieldMIDI. getHitStrengthLayer(hitADCData);
            //Send note on message, note drum timbre on channel 10
            shieldMIDI.setNoteOn (noteNumber, velocity, channel);  
            
        }      
    }
}
