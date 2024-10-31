#pragma once

#ifndef DRUMPAD_H
#define DRUMPAD_H

#include "lerp.h"
#include <cmath>
#include <cstdint>
#include "env.h"

//number of keys
#define numNotes 6
//number of keys with pitch envelope
#define numPitch 3
//variable for logarithmic curve
#define logCon 10.5

//amount pitch scaling is affected by note freq
#define pScale 0.1


//
//note data struct
#define STATE_OFF 0
#define STATE_ATT 1
#define STATE_HOLD 2
#define STATE_REL 3
#define lerpAmt 0.01

//treshold for a note being on or off
static uint8_t noteTresh = 40;
//note struct
typedef struct
{
	float z;
	uint8_t state;
    bool isActive;
    lerpF noteLerp;
}noteData;







class drumPad
{
private:
    /* data */
    uint8_t numActive = 0;
    //pointer to the rx_buffer used to recieve note data
    uint8_t* _rx_buffer;

    float logConvert(uint8_t val);

    float sampleRate;
    env* noteEnvs[numNotes];
    env* freqEnvs[numNotes];
    env* tonEnvs[numNotes];


    float pAmt = 1.0f;



public:
    //init the class with a pointer to the rx buffer and set it to the private _rx_buffer variable for easy acess to the note data send over spi
    drumPad(uint8_t* rx_buff/* args */):_rx_buffer(rx_buff){}
    ~drumPad();

    void updateZ();
    void Init(float sr,env envs[],env pEnvs[],env tEnvs[]);
    void Process();
    noteData notes[numNotes];
    void setPAmt(float p);


};













#endif