#include "daisy_seed.h"
#include "daisysp.h"
#include <stdio.h>
#include <string.h>
#include "dev/oled_ssd130x.h"

#include "per/spi.h"
//timer 
#include "per/tim.h"

#include "drumPad.h"
#include "table.h"
#include "env.h"


using namespace daisy;
using namespace daisysp;

DaisySeed hw;
using MyOledDisplay = OledDisplay<SSD130xI2c128x64Driver>;
MyOledDisplay display;

#define numPot 6
#define potRes 4


#define buffSize 6

SpiHandle dmaSpi;
SpiHandle::Config spi_config;

// buffer for receiving data

uint8_t DMA_BUFFER_MEM_SECTION rx_buffer[numNotes];
uint8_t myNotes[numNotes];

drumPad thisDrumpad(rx_buffer);


//array of wavetables for each voice

float baseTones[numNotes] = {205.3,396.6,304.4,552.7,800,540};
table myTables[numNotes];
float basMult =  0.5;
float bassFreq = 200.0f;
float morph = 1.0f;
float dist = 1.0f;
float offset = 1.0f;
env ampEnvs[numNotes];
env pitchEnvs[numNotes];
env toneEnvs[numNotes];

//for display
float myTableFreqs[numNotes];

float globalRel = 1.0f;
#define relScale 0.05

float pEnvAmount = 1.0f;
#define pScale 1.0
#define highOffset 1.0

float tanhDistortion(float input, float gain) {
    // Apply gain to the input signal
    float amplifiedInput = input * gain;

    // Apply tanh function to create distortion
    float distortedOutput = std::tanh(amplifiedInput);

    return distortedOutput;
}

float noiseGen()
{
	return (((float)rand()/RAND_MAX)* 2.0f) -1.0f;
}


static void callbackFunction(void* context, SpiHandle::Result result)
{
	SpiHandle* transport = static_cast<SpiHandle*>(context);
	
	if(result == SpiHandle::Result::OK)
	{
		// for(uint8_t i = 0;i < numNotes;i++)
		// {
		// 	// myNotes2[i].z = rx_buffer[i];
		// }
		// notes* currentNote = &myNotes[rx_buffer[NOTE_ID]];

		// currentNote->state = rx_buffer[NOTE_ID];
		// currentNote->x = rx_buffer[NOTE_X];
		// currentNote->y = rx_buffer[NOTE_Y];
		// currentNote->z = rx_buffer[NOTE_Z];

		transport->DmaReceive(rx_buffer,buffSize,nullptr,callbackFunction,context);
	}
	else
	{
		//transport->DmaReceive(rx_buffer,buffSize,nullptr,callbackFunction,context);
	}

}

//timer setup
TimerHandle timer;
uint8_t potVal[numPot];
uint8_t PrevpotVal[numPot];

#define displayX 128
#define displayY 64
#define displayRectSize 5

//timer callback function for updating oled display
static void oledTimer(void* context)
{
		display.Fill(false);
		// for(uint8_t i = 0; i < numPot;i++)
		// {
		//    	char strbuff[4];
		// 	display.SetCursor(0,10 * i);
    	// 	snprintf(strbuff, potRes, "%d", potVal[i]);
        // 	display.WriteString(strbuff, Font_7x10, true);
		// }


		float  maxFrequency = myTableFreqs[5];
		for(uint8_t i = 0; i < numNotes;i++)
		{
			int xVal =  static_cast<int>(static_cast<float>(myTableFreqs[i]) / maxFrequency * 128);
			display.DrawLine(xVal,0,xVal,displayY,true);

		}
       
    


    	// Format the float value into the strbuff

        display.Update();
}


void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		thisDrumpad.Process();

		float noise = noiseGen();

		float output = 0;
		for(int i = 0; i < numNotes;i++)
		{
			if(thisDrumpad.notes[i].state != STATE_OFF)
			{
				float ampE = ampEnvs[i].process();
				float pitchE =  pitchEnvs[i].process();
				float toneE = toneEnvs[i].process();
				float sig = myTables[i].process(1.0f +pitchE ,toneE + (morph * (toneE + (std::fmin(thisDrumpad.notes[i].z +ampE,1.0f))))) * (ampE + (thisDrumpad.notes[i].z));
				sig += ((noise * toneE) * 0.05f);
				output += tanhDistortion(sig,dist);
			}
		}
		

		out[0][i] = out[1][i] = output * 0.15f;
	}
}

int main(void)
{
	hw.Init();


	//display stetup
	/** Configure the Display */
    MyOledDisplay::Config disp_cfg;
    // disp_cfg.driver_config.transport_config.pin_config.dc    = hw.GetPin(9);
    // disp_cfg.driver_config.transport_config.pin_config.reset = hw.GetPin(30);
    disp_cfg.driver_config.transport_config.i2c_config.pin_config.scl = hw.GetPin(11);
    disp_cfg.driver_config.transport_config.i2c_config.pin_config.sda = hw.GetPin(12);
    disp_cfg.driver_config.transport_config.i2c_config.speed = I2CHandle::Config::Speed::I2C_100KHZ;
    disp_cfg.driver_config.transport_config.i2c_address = 0x3C;

    /** And Initialize */
    display.Init(disp_cfg);


	//spi setup
	//spi setup
	spi_config.periph = SpiHandle::Config::Peripheral::SPI_1;
    spi_config.mode   = SpiHandle::Config::Mode::SLAVE;
    spi_config.direction  = SpiHandle::Config::Direction::TWO_LINES_RX_ONLY;
    spi_config.datasize       = 8;
    spi_config.clock_polarity = SpiHandle::Config::ClockPolarity::LOW;
    spi_config.clock_phase    = SpiHandle::Config::ClockPhase::ONE_EDGE;
    spi_config.nss            = SpiHandle::Config::NSS::HARD_INPUT;
    spi_config.baud_prescaler = SpiHandle::Config::BaudPrescaler::PS_128;
    // SPI pin config
    spi_config.pin_config.sclk = {DSY_GPIOG, 11};
    spi_config.pin_config.miso = {DSY_GPIOX, 0};
    spi_config.pin_config.mosi = {DSY_GPIOB, 5};
    spi_config.pin_config.nss  = {DSY_GPIOG, 10};

	dmaSpi.Init(spi_config);

	dmaSpi.DmaReceive(rx_buffer,buffSize,nullptr,callbackFunction,&dmaSpi);

	//pot setup
	AdcChannelConfig adc[numPot]; //array size for number of ADC channels you need
	adc[0].InitSingle(hw.GetPin(15));
	adc[1].InitSingle(hw.GetPin(16));
	adc[2].InitSingle(hw.GetPin(17));
	adc[3].InitSingle(hw.GetPin(18));
	adc[4].InitSingle(hw.GetPin(19));
	adc[5].InitSingle(hw.GetPin(20));

	hw.adc.Init(adc, 6); //my DaisySeed instance is called hw
	hw.adc.Start();

	//oled timer setup for periodically updating oled display at 500ms

	TimerHandle::Config tim_config;
	tim_config.enable_irq = true;
	tim_config.dir = TimerHandle::Config::CounterDir::UP;
	tim_config.periph = TimerHandle::Config::Peripheral::TIM_5;
    tim_config.period = 9999;
	timer.Init(tim_config);
	timer.SetPrescaler(99999);
	//set timer to refresh oled after each count resets
	timer.SetCallback(oledTimer,nullptr);
	timer.Start();



	hw.SetAudioBlockSize(8); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_32KHZ);
	float sampleRate = hw.AudioSampleRate();

	for(int i = 0; i < numNotes;i++)
	{
		myTables[i].init(sampleRate);
		myTables[i].setFreq(200);
		ampEnvs[i].init(sampleRate);
		pitchEnvs[i].init(sampleRate);
		pitchEnvs[i].setAtt(0.1f);
		toneEnvs[i].init(sampleRate);
		toneEnvs[i].setAtt(1.0f);


	}

	thisDrumpad.Init(sampleRate,ampEnvs,pitchEnvs,toneEnvs);


	hw.StartAudio(AudioCallback);
	while(1) 
	{
    	// float value = hw.adc.GetFloat(0) ;  // Assuming hw.adc.GetFloat(0) returns a float
	    // int intValue = (int)(value * 128);
		for(uint8_t i = 0; i < numPot;i++)
		{
			uint8_t intValue = (uint8_t)(hw.adc.GetFloat(i) * 255);
			potVal[i] = intValue;
			// if(PrevpotVal[i] != intValue)
			// {
			// 		myTables[0].setFreq( 400 * hw.adc.GetFloat(i));

			// }

			//check to see weather value has changed and update if nececarry
			if(i == 0)
			{
				float knobRead = hw.adc.GetFloat(i);
				if(knobRead < 0.5f)
				{
					morph = knobRead * 2.0f;
					dist = 1.0f;
				}
				else
				{
					morph = 1.0f;
					dist = 1.0f + (knobRead - 0.5f) * 15.0f;
				}
			}
			if(i == 1)
			{
				pEnvAmount = (hw.adc.GetFloat(i) * 8.0f);
			}
			if(i == 2)
			{
				bassFreq = (hw.adc.GetFloat(i) * 400.0f);
			}
			if(i ==3)
			{
				basMult = (hw.adc.GetFloat(i) * 3.0f);
			}
			if(i == 4)
			{
				offset = (hw.adc.GetFloat(i) * 0.666f) - 0.333f;
			}
			if(i == 5)
			{
				globalRel = 0.61 + hw.adc.GetFloat(i) * 8.0f;

			}
		}
		for(int i = 0; i < numNotes;i++)
		{
			float shift;
			if(i%2 == 0)
			{
				shift = offset;
			}
			else
			{
				shift = -offset;
			}
			

				myTableFreqs[i] = (bassFreq + (bassFreq * ( shift +((basMult) * i))));

				myTables[i].setFreq(myTableFreqs[i]);
				ampEnvs[i].setRel(globalRel - (i * (relScale)));
				ampEnvs[i].setAtt((globalRel - (i * (relScale)))/2.0f);

			pitchEnvs[i].setAtt(0.05f);
			thisDrumpad.setPAmt(pEnvAmount);
		}

	}
}
