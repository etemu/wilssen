#if defined(ARDUINO) && ARDUINO >= 100 //check if using Arduino or Wiring
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "RGBlink.h"

LED::LED(int r_pin,int g_pin,int b_pin)
{
	pinMode(r_pin, OUTPUT);
	pinMode(g_pin, OUTPUT);
	pinMode(b_pin, OUTPUT);

	pins[0] = r_pin;
	pins[1] = g_pin;
	pins[2] = b_pin;

	isOn = true;
}

void LED::on()
{
	isOn = true;
	for (int i = 0; i < 3; ++i)
	{
		analogWrite(pins[i],color[i]);
	}
}

void LED::off()
{
	isOn = false;
	for (int i = 0; i < 3; ++i)
	{
		digitalWrite(pins[i],0);
	}
}

void LED::setColor(int r_val, int g_val, int b_val)
{
	color[0] = r_val;
	color[1] = g_val;
	color[2] = b_val;	
	if (isOn)
		on();
}


void LED::selftest()
{
	for (int i = 0; i < 255; ++i)
	{
		setColor(255,i,0);
		on();
		delay(5);
	}
	for (int i = 0; i < 255; ++i)
	{
		setColor(255-i,255,0);
		on();
		delay(5);
	}
	for (int i = 0; i < 255; ++i)
	{
		setColor(0,255,i);
		on();
		delay(5);
	}
	for (int i = 0; i < 255; ++i)
	{
		setColor(0,255-i,255);
		on();
		delay(5);
	}
	for (int i = 0; i < 255; ++i)
	{
		setColor(i,0,255);
		on();
		delay(5);
	}
	for (int i = 0; i < 255; ++i)
	{
		setColor(255,0,255-i);
		on();
		delay(5);
	}		
}