#include "drumPad.h"


drumPad::~drumPad()
{
}


void drumPad::Init(float sr,env envs[],env pEnvs[],env tEnvs[])
{
    sampleRate = sr;

    //set all notes to 0 velocity, and states to off
    for(int i = 0; i < numNotes; i++)
    {
        notes[i].state = STATE_OFF;
        notes[i].z = 0;
        notes[i].isActive = false;
        notes[i].noteLerp.setAmt(lerpAmt);

        noteEnvs[i] = &envs[i];
        freqEnvs[i] = &pEnvs[i];
        tonEnvs[i] = &tEnvs[i];


        //env init
        // noteEnvs[i].init(sampleRate);
        // notes[i].noteSegment.Init(sr);
    }
}

void drumPad::Process()
{
    //first iterate through all notes setting them to the correct values
    for(int i = 0;i < numNotes;i++)
    {   
        //set note value to most recent value from the spi note data buffer
        //if state is off or in release phase and z value is over trigger threshold then set note to active and trigger the note starting its attack phase
        if((notes[i].state == STATE_OFF || notes[i].state == STATE_REL) && _rx_buffer[i] > noteTresh)
        {
            notes[i].state = STATE_HOLD;
            notes[i].z = notes[i].noteLerp.process(logConvert(_rx_buffer[i]));
            float envVol = notes[i].z;
            noteEnvs[i]->trigger(envVol);
            freqEnvs[i]->trigger(pAmt);
            tonEnvs[i]->trigger(0.3f);

            //start attack phase
        }
        //next check to see weather the note is being held, if its held and the buffer value falls below the note treshold then set the note to release phase, otherwise continue linear interpolating note data
        else if(notes[i].state == STATE_HOLD)
        {
            if( _rx_buffer[i] < noteTresh)
            {
                notes[i].state = STATE_REL;
                //take previous reading before going beneath threshold and start the release phase
                noteEnvs[i]->release(notes[i].z);
                //then set note z value to 0
                notes[i].z = 0.0f;

            }
            else
            {
                notes[i].z = notes[i].noteLerp.process(logConvert(_rx_buffer[i]));
            }
        }
        else if(notes[i].state == STATE_REL)
        {
            if(noteEnvs[i]->thisSegment.updateState == true)
            {
                //note has faded out, set state to off
                notes[i].state = STATE_OFF;
            }
        }
        //if note is being held and falls below note threshold start release phase
    }
}


float drumPad::logConvert(uint8_t val)
{
    float normalizedValue = static_cast<float>(val) / 255.0f;
    float logarithmicValue = logf(normalizedValue * (logCon - 1) + 1) / logf(logCon);
    return logarithmicValue;
}

void drumPad::setPAmt(float p)
{
    pAmt = p;
}
