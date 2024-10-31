#include "table.h"

float table::table0[tableSize];
float table::table1[tableSize];

void table::init(float sampleRate)
{
    sampleRate_ = sampleRate;


    bufferPos = 0;
    setFreq(45.0f);
    currentTable = table0;
    createTable(tableSize);
    createTableSin2(tableSize);
    // createTableSquare(tableSize);
    // createTableSquare(tableSize);
    setTable(1);
}
void table::createTable(int tSize)
{

        // int newTableSize = tableSize * 2;

        for (int i = 0; i < tableSize; i++) {
            float phase = (i * 2 * M_PI) / tableSize; // Calculate the phase angle
            table0[i] = (sin(phase)); // Compute the sine of the phase angle
        }


    
}

void table::createTableSin2(int tSize)
{

        #define mix 0.25
        for (int i = 0; i < tableSize; i++) {
            float phase = (i * 2 * M_PI) / tableSize; // Calculate the phase angle

            table1[i] = (0.2f * sin(7.0f * phase)) +  (0.2f * sin(4.0f * phase)) + ((0.4) * sin(1.0f * phase)) + ((0.4f) * sin(3.0f * phase)) + ((0.05f) * sin(5.0f * phase)); // Compute the sine of the phase angle
        }
    
}





void table::setSquare()
{
    createTableSquare(tableSize);
}
void table::setSin()
{
    createTable(tableSize);
}
void table::createTableSquare(int tSize)
{

        
        for (int i = 0; i < tableSize; i++)
        {

            float amt;
            if(i > tableSize/2)
            {
             amt = 1.0f;
            }
            else
            {
            amt = -1.0f;
            }
            table1[i] = amt;// Compute the sine of the phase angle
        }
    

}

void table::createTableSaw(int tSize)
{

    for (int i = 0; i < tableSize; i++)
    {
        float amt = (float)i / (float)tableSize;  // Ensure floating-point division
        table0[i] = 2 * amt - 1;  // Map to the range [-1, 1]
    }
}

void table::createTableTri(int tSize)
{

    for (int i = 0; i < tableSize; i++)
    {
        // Normalize the index to the range [0, 1]
        float normalizedIndex = (float)i / tableSize;

        // Calculate the triangle wave value
        float amt = 1.0f - 4.0f * fabs(normalizedIndex - 0.5f);

        // Map to the range [-1, 1]
        amt = 2.0f * amt - 1.0f;

        // Store the computed value in the table
        table0[i] = amt;
    }
}


void table::setFreq(float freq)
{
    bufferInc = (freq * tableRez) / sampleRate_;
}

float table::process(float env,float m)
{
    if(m > 1.0f) m = 1.0f;
    bufferPos += bufferInc * env;
    //reset the position while keeping any float values 
    if(bufferPos >= tableRez)
    {
        bufferPos -= tableRez;
    }

    //interpolate between two frames in the wavetable
    return lerpF(bufferPos,m);

}

void table::setTable(int t)
{
    switch(t)
    {
        case 0: currentTable = table0;
        break;
        case 1: currentTable = table1;
        break;
    }
}

void table::setNewTable(int t)
{
    switch (t)
    {
    case 0:
        createTable(tableSize);/* code */
    break;

    case 1:
    createTableSquare(tableSize);
    break;

    case 2:
    createTableSaw(tableSize);
    break;
    case 3:
    createTableTri(tableSize);
    break;
    }

}



float table::getTable(int index)
{
    return table0[index];
}



float table::lerpF(float pos,float morph)
{

	
	// Linear interpolation is best done simply as:
	// g(x) = y[0] + x(y[1] − y[0])
	//http://yehar.com/blog/wp-content/uploads/2009/08/deip.pdf


	//pos between 0 and 128

    //check the range of position and reset if neccecary

	int posFloor = floor(pos);
	float interp = pos - posFloor;

    float frame1 = table0[posFloor];
    //if on last frame then set frame 2 to first frame
    float frame2 = (posFloor == tableRez - 1) ? table0[0] : table0[posFloor + 1];

    float frame1a = table1[posFloor];
    //if on last frame then set frame 2 to first frame
    float frame2a = (posFloor == tableRez - 1) ? table1[0] : table1[posFloor + 1];

    

    //return (frame1 + (interp * (frame2 - frame1)));

    float table1T = (frame1 + (interp * (frame2 - frame1)));
    float table2T = (frame1a + (interp * (frame2a - frame1a)));

    return (table1T + (morph * (table2T - table1T)));


	// int morphFloor = floor(morph);
	// float morphInterp = morph - morphFloor;

    
    // Wavetable.tablePtr;
	
	//interpolate between the two waveforms selected with moprh

	// int position1 = tableRez * morphFloor + posFloor;
	// int position2 = position1 + tableRez;

	// float table1 = Wavetable::tablePtr[position1] + posInt * (Wavetable::tablePtr[position1 + 1] - Wavetable::tablePtr[position1]);
	// float table2 = Wavetable::tablePtr[position2] + posInt * (Wavetable::tablePtr[position2 + 1] - Wavetable::tablePtr[position2]);

	//return the result after attenuating with the morphInterp value;
	// return (table1 * (1 - morphInterp) + (table2 * morphInterp));

}

