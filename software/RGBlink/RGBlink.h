#ifndef RGBlink_h //Avoid including this lib twice
#define RGBlink_h

#if defined(ARDUINO) && ARDUINO >= 100 //check if using Arduino or Wiring
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif


// predefined RGB colors 

#define red 	255,000,000  //TODO: specific header just for this
#define green 	000,255,000
#define blue 	000,000,255
#define yellow 	255,255,000
#define white 	255,255,255
#define black	000,000,000

/* struct to store frame data for patterns, currently not needed
typedef struct { 
	int r;
	int g;
	int b;
	int t;
} frame;
*/

class LED // main class
{
	public:
		LED(int _r_pin,int _g_pin,int _b_pin);	

		void on();
		void off();
		
		void selftest();
		void update();	

		void initDefaults();	

		void setColor(int _r_val, int _g_val, int _b_val);
		void setBlink(int _on_val,int _off_val);
		void setMode(int _mode);
		int getMode();
	private:
		extern int pins[3];	// used Arduino Pins
		extern int color[3]; 	// color buffer {red, green, blue}
		extern int blink[2];	

		int mode;
		bool isOn;	

		unsigned long prevMillis; // store the last mills();
};


#endif