#include <RGBlink.h>

/* 
 * define your RGB-LED like this:
 * LED led(red_pin,green_pin,blue_pin);
 * pins have to be PWM capable.
 */

LED led(11,9,10);

void setup()
{ 
	/* 
	 * There are 3 LED modes:
	 * 0: static color
	 * 1: blink (on/off)
	 * 2: pulse (like blink, just w/ soft transitions)
	 */

	led.setMode(1); // set the mode to normal blinking

	/* set the blink delays
	 * setBlink(on_time,off_time), time in milliseconds
	 * this is optional, by default the delay is (1000,1000)
	 */

	led.setBlink(2000,2000);

	/* 
	 * set the color
	 * you can either use setColor(0..255,0..255,0..255) to set custom RGB values
	 * or use a predefined color like red, green, yellow setColor(red)
	 */
	 
	led.setColor(red);	// in this case we set the color to red 
}

void loop() 
{
	led.update();	// call the leds update cycle for non-blocking blinking

	/* 
	 * Add your own code here
	 * (should be non blocking or just using delays<20ms)
	 */
}