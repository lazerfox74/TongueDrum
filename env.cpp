#include "env.h"

env::env(/* args */)
{
}

env::~env()
{
}

void env::init(float sampleRate)
{
    _sampleRate = sampleRate;
    thisSegment.Init(sampleRate);
    eState = 0;
}

float env::process()
{
    if(eState == 0)
    {

    }
    else if(eState ==1 && thisSegment.updateState == true)
    {
        thisSegment.rampTo(values[1],times[1]);
        thisSegment.updateState = false;
        // eState ++;
    }
    else if(eState ==2)
    {
        thisSegment.rampTo(values[2],times[2]);
        thisSegment.updateState = false;
        eState ++;
    }
    else if(eState ==3 && thisSegment.updateState ==true)
    {
        //if release phase finished tell the drumpad that the note is off so cpu can be saved

    }
    return thisSegment.process();
    // else if(eState ==2 && thisSegment.updateState == true)
    // {
    //     thisSegment.rampTo(values[1],times[1]);
    //     thisSegment.updateState = false;
    //     eState ++;
    // }
    // else if(eState ==3)
    // {
    //     //start decay
    //     // thisSegment.setValue(2);
    //     thisSegment.rampTo(values[2],times[2]);
        
    //     allowDecay = true;
    // }


}

void env::trigger(float vel)
{
    eState = 1;
    thisSegment.updateState = true;
    thisSegment.setValue(vel);
}


void env::release(float vel)
{
    eState = 2;
    thisSegment.updateState = true;
    thisSegment.setValue(vel + thisSegment.currentVal_);
}

void env::setRel(float rel)
{
    times[2] = rel;
}

void env::setAtt(float att)
{
    times[1] = att;
}

