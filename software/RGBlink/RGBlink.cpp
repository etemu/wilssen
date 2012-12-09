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

LED::LED(uint8_t to_red_pin,uint8_t to_green_pin,uint8_t to_blue_pin)
{
	red_pin = to_red_pin;
	green_pin = to_green_pin;
	blue_pin = to_blue_pin;
    
    initDefaults();
}

void LED::initDefaults()
{
	isOn = true;
	setColor(white);
    
	blink_on = 1000;  // basic blink pattern (1 sec off/1 sec on)
	blink_off = 1000;
	mode = 0;

	prevMillis = 0;
}

/***************************
 * Basic operations
 ***************************/

void LED::on()
{
	isOn = true;

	analogWrite(red_pin, red_val_orig);
	analogWrite(green_pin, green_val_orig);
	analogWrite(blue_pin, blue_val_orig);

}

void LED::off()
{
	isOn = false;

	analogWrite(red_pin, 0);
	analogWrite(green_pin, 0);
	analogWrite(blue_pin, 0);
}
/* TODO: reimplement
void LED::selftest()  //TODO: change this to create a pattern
{
	//loop trough all colors maybe?	
}
*/
/***************************
 * update cycle for patterns
 ***************************/

void LED::update()
{
	switch(mode)
	{	
		case 1: //blink
		{
			unsigned long currentMillis = millis();
		   	if(isOn && (uint16_t)(currentMillis - prevMillis) >= blink_on)//blink_on)  // a boolean 'true' equals '1', 'false' equals '0'
		   	{
		   		off();
		   		prevMillis = currentMillis;
		   	}

		   	else if(!isOn && (uint16_t)(currentMillis - prevMillis) >= blink_off)//blink_off)  // a boolean 'true' equals '1', 'false' equals '0'
		   	{
		   		on();
		   		prevMillis = currentMillis;
		   	}
			break;
		}	

		case 2: //fade
		{
			unsigned long currentMillis = millis();
			uint16_t blink_diff = (uint16_t)(currentMillis - prevMillis);

			if(isOn)
		   	{
		   		float perc = (float)blink_diff / (float)blink_on;
		   		if(perc >= 1)
		   		{ 
		   			off();
		   			prevMillis = currentMillis;
		   		}
		   		else
		   		{
		   			writeColor((uint8_t)(perc*red_val_orig),
		   					   (uint8_t)(perc*green_val_orig),
		   					   (uint8_t)(perc*blue_val_orig));
		   		}
		   	}

		   	else if(!isOn)
		   	{
		   		float perc = (float)blink_diff / (float)blink_off;

		   		if(perc >= 1)
		   		{ 
		   			on();
		   			prevMillis = currentMillis;
		   		}
		   		else
		   		{
		   			writeColor(red_val_orig-(uint8_t)(perc*red_val_orig),
		   					   green_val_orig-(uint8_t)(perc*green_val_orig),
		   					   blue_val_orig-(uint8_t)(perc*blue_val_orig));
		   		}
		   		
		   	}
		   	
			break;	
		}

		//default:
	    	//do nothing
	}
}

/***************************
 *	Setter/Getter methods
 ***************************/

 void LED::setBlink(uint16_t to_on_val, uint16_t to_off_val)
{
	blink_off = to_off_val;
	blink_on =	to_on_val;
}

void LED::writeColor(uint8_t to_red_val, uint8_t to_green_val, uint8_t to_blue_val)  // show a color w/out deleting the buffer, needed for fading
{
	red_val_curr =	to_red_val;
	green_val_curr = to_green_val;
	blue_val_curr = to_blue_val;

	analogWrite(red_pin, red_val_curr);
	analogWrite(green_pin, green_val_curr);
	analogWrite(blue_pin, blue_val_curr);	
}

void LED::setColor(uint8_t to_red_val, uint8_t to_green_val, uint8_t to_blue_val)
{
	red_val_orig =	to_red_val;
	green_val_orig = to_green_val;
	blue_val_orig = to_blue_val;

	if (isOn) 
		on();
}

void LED::setMode(uint8_t to_mode)
{
	mode = to_mode;
}

int LED::getMode()
{
	return mode;
}
