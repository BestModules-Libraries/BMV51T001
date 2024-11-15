/***************************************************************************
File:       		  BMV51T001.h
Author:           BEST MODULES CORP.
Description:      Define classes and required variables
Version:          V1.0.2   -- 2024-11-15
****************************************************************************/
#ifndef  _BMV51T001_H
#define  _BMV51T001_H

#include "Arduino.h"
#include "BM_MIDIDefine.h"

#define    BMV51T001_KEYBOARD_ENABLE    (1)
#define    BMV51T001_KEYBOARD_DISABLE   (0)
#define    BMV51T001_HIT_ENABLE         (1)
#define    BMV51T001_HIT_DISABLE         (0)

/*****************class for the BMV51T001*******************/
class BMV51T001
{
  public:
    BMV51T001();
	  void begin(void);
    bool setVolume(uint8_t volume);
    bool setHitSensitivity(uint8_t value);
    bool setHitTimeInterval(uint8_t value);
    void setHitStrengthLayer(uint8_t layers);
    bool setKeyboardOut(uint8_t status);
    bool setHitOut(uint8_t status);
    void scanKeyboard(void);
    void scanHit(void);
    bool isKeyboard(void);
    bool isHit(void);
    void readKeyboardData(uint8_t keyBuf[]);
    uint16_t readHitADCData(void);
    uint8_t getHitStrengthLayer(uint16_t hitADCData);
    bool reset(void);
    uint8_t getFWVer(void);
	/* MIDI func */	
    void setNoteOn(uint8_t noteNumber, uint8_t velocity, uint8_t channel);
    void setNoteOff(uint8_t noteNumber, uint8_t velocity, uint8_t channel);
    void setTone(uint8_t toneNumber, uint8_t channel);
    void setChannelVolume(uint8_t channelVolume, uint8_t channel);
    void setPitchBend(int16_t pitchValue, uint8_t channel);

  private:
    void writeBytes(uint8_t wbuf[], uint8_t wlen);
    bool readBytes(uint8_t rbuf[], uint8_t wlen);
    void sendMIDI(MidiType type, uint8_t data1, uint8_t data2, uint8_t channel);
    uint8_t   _mRunningStatus_TX;
    bool      _mRunningStatus;//true:use running status；false:not use runniing ststua
    uint8_t   _hitBuf[2];
    uint8_t   _keyBuf[8];
    uint16_t  _hitADCData;
    uint8_t   _strength;
    uint8_t   _strengthLayers;
    uint8_t   _isKeyFlag;//1：That means there's key data coming in
    uint8_t   _isHitFlag;//1：That means there's hit data coming in
    uint8_t   _firstData;
    uint8_t   _firstIn;
};
#endif


