#pragma once

#ifndef TABLE_H
#define TABLE_H

#include "cmath"

#define tableSize 512
// #define tableRez 128
#define tableRez 512
#define M_PI 3.14159265358979323846



class table
{
    public:

    void init(float sampleRate);
    void setFreq(float freq);
    float process(float env,float m);

    //0 = square 1 = sine
    void setTable(int t);
    void setNewTable(int t);



    void setSquare();
    void setSin();

    float getTable(int index);


    private:
    void createTableSaw(int tSize);
    void createTable(int tSize);
    void createTableSquare(int tSize);
    void createTableTri(int tSize);
    void createTableSin2(int tSize);


    static float table0[tableSize];
    static float table1[tableSize];

    float* currentTable;

    float bufferInc;
    float bufferPos;
    float sampleRate_;


    float lerpF(float pos,float morph);

};

#endif