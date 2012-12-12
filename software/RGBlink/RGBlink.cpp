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


LED::LED(uint8_t to_red_pin,uint8_t to_green_pin,uint8_t to_blue_pin) // init the LED w/out setting the inverted param
{
	red_pin = to_red_pin;
	green_pin = to_green_pin;
	blue_pin = to_blue_pin;
    
	initDefaults();

	inverted = false;
}

LED::LED(uint8_t to_red_pin,uint8_t to_green_pin,uint8_t to_blue_pin,bool to_inverted) // init the LED w/ setting the inverted param
{
	red_pin = to_red_pin;
	green_pin = to_green_pin;
	blue_pin = to_blue_pin;
    
    initDefaults();

	inverted = to_inverted;    
}
void LED::initDefaults()
{
	isOn = true;
	setColor(red);
    
	blink_on = 1000;  // basic blink pattern (1 sec off/1 sec on)
	blink_off = 1000;
	mode = 0;

	prevMillis = 0;
}

/***************************
 * Basic operations
 ***************************/

 // NOT needen, replace w/ foo

void LED::on()
{
	isOn = true;
	writeHSB(color);
}

void LED::off()
{
	isOn = false;

	writeRGB((RGB){0,0,0});
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
		   			isOn = false;
		   			prevMillis = currentMillis;
		   		}
		   		else
		   		{
		   			HSB newCol = {color.hue,color.sat,(uint8_t)(perc*color.bri)};
		   			writeHSB(newCol);
		   		}
		   	}

		   	else if(!isOn)
		   	{
		   		float perc = (float)blink_diff / (float)blink_off;

		   		if(perc >= 1)
		   		{ 
		   			isOn = true;
		   			prevMillis = currentMillis;
		   		}
		   		else
		   		{
		   			HSB newCol = {color.hue,color.sat,255-(uint8_t)(perc*color.bri)};
		   			writeHSB(newCol);
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

void LED::writeRGB(RGB to_color)  // show a color w/out deleting the buffer, needed for fading
{
	
	if(inverted)
	{
		analogWrite(red_pin, ~to_color.red);    // "~" is a bitshift operator and will invert the given value (255->0,100->155)
		analogWrite(green_pin, ~to_color.green);
		analogWrite(blue_pin, ~to_color.blue);		
	}
	else
	{   
		analogWrite(red_pin, to_color.red);
		analogWrite(green_pin, to_color.green);
		analogWrite(blue_pin, to_color.blue);
	}
}

void LED::writeHSB(HSB to_color)  // show a color w/out deleting the buffer, needed for fading
{
	writeRGB(HSBtoRGB(to_color));	
}

void LED::setColor(HSB to_color)
{
	color = to_color;
	if (isOn && mode == 0) // prevent performance issues when fading while setting color 
		writeHSB(color);
}

void LED::setMode(uint8_t to_mode)
{
	mode = to_mode;
}

int LED::getMode()
{
	return mode;
}

/* NOT needed, replaced w/ HSB
RGB mix(RGB color_1, RGB color_2, uint8_t step)
{
	float perc = (float)step/255;

	RGB newCol = {(uint8_t)(perc*color_1.red)	+ (uint8_t)((1-perc)*color_2.red),
		   		  (uint8_t)(perc*color_1.green)	+ (uint8_t)((1-perc)*color_2.green),
		   		  (uint8_t)(perc*color_1.blue)	+ (uint8_t)((1-perc)*color_2.blue)};

	return newCol;
}
*/
RGB HSBtoRGB(HSB from_color) 
{ 
	/*
	 * Hue: 0..359
	 * Sat: 0..255
	 * bri: 0..255
	 */

	RGB result;
 	from_color.bri = pgm_read_byte(&dim_curve[from_color.bri]);
 	from_color.sat = 255-pgm_read_byte(&dim_curve[255-from_color.sat]);

	uint8_t r, b, g, base;

  	if (from_color.sat == 0) //Acromatic, from_color.hue not needed
  	{ 
    	result.red = from_color.bri;
	    result.green = from_color.bri;
	    result.blue = from_color.bri; 
	} 
	else  
	{ 
		base = ((255 - from_color.sat) * from_color.bri)>>8;

	    switch(from_color.hue/hstep) 
	    { 
		    case 0:// 0..59
		    {
		        result.red = from_color.bri;		
		        result.green = (((from_color.bri-base)*from_color.hue)/hstep)+base;		
		        result.blue = base;		
		    	break;
			}

		    case 1://hstep..119
		    {
		        result.red = (((from_color.bri-base)*(hstep-(from_color.hue%hstep)))/hstep)+base;		
		        result.green = from_color.bri;		
		        result.blue = base;		
		    	break;
		    }

		    case 2://120..179
		    {
		        result.red = base;		
		        result.green = from_color.bri;		
		        result.blue = (((from_color.bri-base)*(from_color.hue%hstep))/hstep)+base;		
		    	break;
			}

			case 3://180..239
		    {
		        result.red = base;		
		        result.green = (((from_color.bri-base)*(hstep-(from_color.hue%hstep)))/hstep)+base;		
		        result.blue = from_color.bri;		
		    	break;
		    }

			case 4://240..299
		    {
		        result.red = (((from_color.bri-base)*(from_color.hue%hstep))/hstep)+base;		
		        result.green = base;		
		        result.blue = from_color.bri;		
		    	break;
		    }

			case 5://300..359
		    {
		        result.red = from_color.bri;		
		        result.green = base;		
		        result.blue = (((from_color.bri-base)*(hstep-(from_color.hue%hstep)))/hstep)+base;		
		    	break;
		    }		
		}
	}   
	return result;
}
