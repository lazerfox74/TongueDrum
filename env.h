#pragma once

#ifndef ENV_H
#define ENV_H

#include "Segment.h"
#include <cstdint>
// #include "drumPad.h"

class env
{
private:
    /* data */
//    noteData* _note;
   uint8_t eState;
   float _sampleRate;
   bool allowDecay = false;

    float values[3] = {0.5f,0.0f,0.0f};

    float times[3] = {0.05f,0.5f,1.9f};
   
public:
    env(/* args */);
    ~env();

    void init(float sampleRate);
    float process();
    void trigger(float vel);
    void release(float vel);
    Segment thisSegment;
    void setRel(float rel);
    void setAtt(float att);
    //set maximum value




};

#endif