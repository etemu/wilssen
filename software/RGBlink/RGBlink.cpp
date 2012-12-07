#if defined(ARDUINO) && ARDUINO >= 100 //check if using Arduino or Wiring
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "RGBlink.h"

/* 
 * Modes:
 * 0 Color
 * 1 Pattern
 * 2 Heartbeat
 */

/***************************
 * Initialization
 ***************************/

LED::LED(int _r_pin,int _g_pin,int _b_pin)
{
	int pins[3] = {_r_pin,_g_pin,_b_pin};
	/*
	pins[0] = _r_pin;
	pins[1] = _g_pin;
	pins[2] = _b_pin;
    */
    for (int i = 0; i < 3; ++i)
    	pinMode(pins[i], OUTPUT);
    
    initDefaults();
}

void LED::initDefaults()
{
	isOn = true;
	setColor(white);
    
    blink = {1000,1000}; //default blink delay (1 sec on, 1 sec off)

	frame_i = 0;
	mode = 1;

	prevMillis = 0;
}

/***************************
 * Basic operations
 ***************************/

void LED::on()
{
	isOn = true;
	for (int i = 0; i < 3; ++i)
		analogWrite(pins[i],color[i]);
}

void LED::off()
{
	isOn = false;
	for (int i = 0; i < 3; ++i)
		analogWrite(pins[i],0);
}

void LED::selftest()  //TODO: change this to create a pattern
{
	//loop trough all colors maybe?	
}

/***************************
 * update cycle for patterns
 ***************************/

void LED::update()
{
	switch(mode)
	{
		case 1:
			unsigned long currentMillis = millis();
		   	if(currentMillis - prevMillis >= blink[(int)isOn])  // a boolean 'true' equals '1', 'false' equals '0'
		   	{
		   		if (isOn)
		   			off();
		   		else
		   			on();

		   		prevMillis = currentMillis;
		   	}

		break;	

		//default:
	    	//do nothing
	}
}

/***************************
 *	Setter/Getter methods
 ***************************/

 void LED::setBlink(int _on_val, int _off_val)
{
	blink = {_off_val,_on_val};
}

void LED::setColor(int _r_val, int _g_val, int _b_val)
{
	color[0] = _r_val;
	color[1] = _g_val;
	color[2] = _b_val;	
	if (isOn)
		on();
}

void LED::setMode(int _mode)
{
	mode = _mode;
}

int LED::getMode()
{
	return mode;
}