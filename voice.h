#pragma once

#ifndef VOCIE_H
#define VOICE_H


class voice
{
private:
    /* data */

    float _sampleRate;
    float freq;
    float amp;
public:
    voice(/* args */);
    ~voice();

    void Init(float sr);
    void setFreq(float f);
};





#endif
