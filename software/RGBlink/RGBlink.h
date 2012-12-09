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

class LED // main class
{
	public:
		LED(uint8_t to_red_pin,uint8_t to_green_pin,uint8_t to_blue_pin);	

		void on();
		void off();
		
		//void selftest();
		void update();	

		void initDefaults();	

		void setColor(uint8_t to_red_val, uint8_t to_green_val, uint8_t to_blue_val);
		void writeColor(uint8_t to_red_val, uint8_t to_green_val, uint8_t to_blue_val);
		void setBlink(uint16_t to_on_val, uint16_t to_off_val);
		void setMode(uint8_t to_mode);
		int getMode();
	private:
		uint8_t red_pin, green_pin, blue_pin;	// used Arduino Pins
		uint8_t red_val_orig, green_val_orig, blue_val_orig; 	// store main colors
		uint8_t red_val_curr, green_val_curr, blue_val_curr; 	// store current color (while fading)
		uint16_t blink_on, blink_off;

		uint8_t mode;
		bool isOn;	

		unsigned long prevMillis; // store the last mills();
};


#endif