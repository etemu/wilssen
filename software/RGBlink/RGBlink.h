
#ifndef RGBlink_h //Avoid including this lib twice
#define RGBlink_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define red 	255,000,000
#define green 	000,255,000
#define blue 	000,000,255
#define yellow 	255,255,000
#define white 	255,255,255


class LED
{
public:
	LED(int r_pin,int g_pin,int b_pin);
	void on();
	void off();
	void setColor(int r_val, int g_val, int b_val);
	void selftest();
private:
	int pins[3];
	int color[3]; 
	bool isOn;
};


#endif