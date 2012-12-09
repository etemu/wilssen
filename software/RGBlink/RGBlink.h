#ifndef RGBlink_h //Avoid including this lib twice
#define RGBlink_h

#if defined(ARDUINO) && ARDUINO >= 100 //check if using Arduino or Wiring
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define gamma_correct 1

typedef struct
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} RGB;

typedef struct
{
	uint16_t hue;
	uint8_t	 saturation;
	uint8_t  brightness;
} HSB;

const RGB red 		= {255,000,000};
const RGB green 	= {000,255,000};
const RGB blue 		= {000,000,255};
const RGB yellow 	= {255,255,000};
const RGB white 	= {255,255,255};
const RGB black		= {000,000,000};


PROGMEM const prog_uint8_t dim_curve[256] = {
    0,   1,   1,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,
    3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   4,   4,   4,   4,
    4,   4,   4,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   6,   6,   6,
    6,   6,   6,   6,   6,   7,   7,   7,   7,   7,   7,   7,   8,   8,   8,   8,
    8,   8,   9,   9,   9,   9,   9,   9,   10,  10,  10,  10,  10,  11,  11,  11,
    11,  11,  12,  12,  12,  12,  12,  13,  13,  13,  13,  14,  14,  14,  14,  15,
    15,  15,  16,  16,  16,  16,  17,  17,  17,  18,  18,  18,  19,  19,  19,  20,
    20,  20,  21,  21,  22,  22,  22,  23,  23,  24,  24,  25,  25,  25,  26,  26,
    27,  27,  28,  28,  29,  29,  30,  30,  31,  32,  32,  33,  33,  34,  35,  35,
    36,  36,  37,  38,  38,  39,  40,  40,  41,  42,  43,  43,  44,  45,  46,  47,
    48,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,
    63,  64,  65,  66,  68,  69,  70,  71,  73,  74,  75,  76,  78,  79,  81,  82,
    83,  85,  86,  88,  90,  91,  93,  94,  96,  98,  99,  101, 103, 105, 107, 109,
    110, 112, 114, 116, 118, 121, 123, 125, 127, 129, 132, 134, 136, 139, 141, 144,
    146, 149, 151, 154, 157, 159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 190,
    193, 196, 200, 203, 207, 211, 214, 218, 222, 226, 230, 234, 238, 242, 248, 255
};

class LED // main class
{
	public:
		LED(uint8_t to_red_pin,uint8_t to_green_pin,uint8_t to_blue_pin);	

		void on();
		void off();
		
		//void selftest();
		void update();	

		void initDefaults();	

		void setColor(RGB to_color);
		void writeColor(RGB to_color);
		void setBlink(uint16_t to_on_val, uint16_t to_off_val);
		void setMode(uint8_t to_mode);
		int getMode();
	private:
		uint8_t red_pin, green_pin, blue_pin;	// used Arduino Pins
		RGB color;
		uint16_t blink_on, blink_off;

		uint8_t mode;
		bool isOn;	

		unsigned long prevMillis; // store the last mills();
};

RGB mix(RGB color_1, RGB color_2, uint8_t step);

#endif