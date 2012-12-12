#include <RGBlink.h>

/* 
 * define your RGB-LED like this:
 * LED led(red_pin,green_pin,blue_pin);
 * pins have to be PWM capable.
 */

LED led(11,9,10);

void setup() 
{
	// no further setup needed
}

void loop()
{
	/* 
	 * set the color
	 * you can either use setColor(0..255,0..255,0..255) to set custom RGB values
	 * or use a predefined color like red, green, yellow setColor(red)
	 */

	led.setColor(red);
	delay(1000);
	led.setColor(yellow);
	delay(1000);
	led.setColor(green);
	delay(1000);
}
