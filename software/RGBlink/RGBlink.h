#ifndef RGBlink_h //Avoid including this lib twice
#define RGBlink_h

#if defined(ARDUINO) && ARDUINO >= 100 //check if using Arduino or Wiring
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define hstep 60  // steps for each main color(cyan, magenta, yellow, green, red , blue), by default 60 => 360° colorspace

/**
 *  Helper class for RGB-colors.
 */

typedef struct
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} RGB;

/**
 *  Helper class for HSB-colors.
 */

typedef struct
{
	uint16_t hue; /**< hue value. */
	uint8_t	 sat; /**< saturation value. */
	uint8_t  bri; /**< brightness value. */
} HSB;

const HSB red 		= {000	  ,255,255}; 
const HSB green 	= {hstep*2,255,255};
const HSB blue 		= {hstep*4,255,255};
const HSB yellow 	= {hstep  ,255,255};

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

/**
 *  Our main class.
 *  This class provides the functions used to drive a RGB-LED.
 */

class LED 
{
	public:
		/**
 		*  Initialize the LED-class.
 		*  This class provides the functions used to drive a RGB-LED
 		*  \param to_red_pin The Arduino pin the LEDs red pin is connected to
 		*  \param to_green_pin The Arduino pin the LEDs green pin is connected to
 		*  \param to_blue_pin The Arduino pin the LEDs blue pin is connected to
 		*/
		LED(uint8_t to_red_pin,uint8_t to_green_pin,uint8_t to_blue_pin);
		/**
 		*  Initialize the LED-class with an invert-option.
 		*  This class provides the functions used to drive a RGB-LED with inverted PWM
 		*  \param to_red_pin The Arduino pin the LEDs red pin is connected to
 		*  \param to_green_pin The Arduino pin the LEDs green pin is connected to
 		*  \param to_blue_pin The Arduino pin the LEDs blue pin is connected to
 		*  \param to_inverted If 0 (false) this is much like the other LED-init, if 1 (true) all output PWM-values are inverted (255-x)
 		*/
		LED(uint8_t to_red_pin,uint8_t to_green_pin,uint8_t to_blue_pin,bool to_inverted);

		/**
 		*  Turn the LED on.
 		*  Converts the current color to RGB and writes it to the pins. 
 		*/
		void on();
		/**
 		*  Turn the LED on.
 		*  Sets all PWM-pins to 0 (255 is the invert-option is set).
 		*/
		void off();

		/**
 		*  Update cycle for non-blocking LED functions.
 		*  Must be called every few milliseconds if you want to use functions like blink, fade or flash
 		*/
		void update();	
		/**
 		*  Reset the LEDs settings to the default values
 		*/
		void initDefaults();
		/**
 		*  Write an RGB value to the PWM-pins w/out changing the color buffer.
 		*/
		void writeRGB(RGB to_color);
		/**
 		*  Write an HSB value to the PWM-pins w/out changing the color buffer.
 		*  like writeRGB(HSBtoRGB(RGB to_color)).
 		*/
		void writeHSB(HSB to_color);
		/**
 		*  Write a HSB color to the buffer.
 		*  And if the LED is on to the PWM-pins.
 		*/
		void setColor(HSB to_color);
		/**
 		*  Set the blink parameters.
 		*  Needed for mode 1 and 2 (blink / pulse)
 		*  \param to_on_val is the time the LED should be on in milliseconds.
 		*  \param to_off_val is the time the LED should be off in milliseconds.
 		*/
		void setBlink(uint16_t to_on_val, uint16_t to_off_val);
		/**
 		*  Set the LEDs mode.
 		*  0: single color
 		*  1: blink
 		*  2: pulse (soft blinking)
 		*  3: selftest (loops trough the HSB color spectrum in 3600 milliseconds)
 		*  4: flash (just needed for the flash() function)
 		*  \param to_on_val is the time the LED should be on in milliseconds.
 		*  \param to_off_val is the time the LED should be off in milliseconds.
 		*/		
		void setMode(uint8_t to_mode);
		/**
 		*  Returns the LEDs current mode (0..4).
 		*/
		int getMode();
		/**
 		*  Flash the LED.
 		*  Sets the LED on and off again after n milliseconds (non-blocking).
 		*  \param to_on_val is the time the LED should flash in milliseconds.
 		*/
		void flash(uint16_t to_on_val);
	private:
		uint8_t red_pin, green_pin, blue_pin; // used Arduino Pins
		HSB color;
		uint16_t blink_on, blink_off, flash_on;
		bool isOn;
		bool inverted; // invert the PWM values?

		uint8_t mode;
	
		unsigned long prevMillis; // store the last mills() for non-block blinking
};

/**
 *  Converts HSB to RGB colors.
 *  \param from_color takes an object of the HSB class and returns its RGB equivalent.
 */
RGB HSBtoRGB(HSB from_color);
/**
 *  Mix two HSB colors (in 256 steps).
 *  \param color_1 the color at step 0.
 *  \param color_2 the color at step 255.
 *  \step the step on the gradient from color_1 to color_2
 *  example: mix(red,green,128) will return something yellow
 */
HSB mix(HSB color_1, HSB color_2, uint8_t step);

#endif
