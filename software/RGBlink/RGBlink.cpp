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
	mode = 1;

	prevMillis = 0;
}

/***************************
 * Basic operations
 ***************************/

void LED::on()
{
	isOn = true;
	writeColor(color);
}

void LED::off()
{
	isOn = false;
	writeColor(black);
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
		   			RGB newCol = {(uint8_t)(perc*color.red),
		   					   	  (uint8_t)(perc*color.green),
		   					   	  (uint8_t)(perc*color.blue)};

		   			writeColor(newCol);
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
		   			RGB newCol = {color.red-(uint8_t)(perc*color.red),
		   					   	  color.green-(uint8_t)(perc*color.green),
		   					   	  color.blue-(uint8_t)(perc*color.blue)};

		   			writeColor(newCol);
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

void LED::writeColor(RGB to_color)  // show a color w/out deleting the buffer, needed for fading
{

	if(gamma_correct)
	{
		analogWrite(red_pin, 	pgm_read_byte(&dim_curve[to_color.red]));
		analogWrite(green_pin,	pgm_read_byte(&dim_curve[to_color.green]));
		analogWrite(blue_pin, 	pgm_read_byte(&dim_curve[to_color.blue]));
	}
	else
	{
		analogWrite(red_pin, to_color.red);
		analogWrite(green_pin, to_color.green);
		analogWrite(blue_pin, to_color.blue);
	}
	
}

void LED::setColor(RGB to_color)
{
	color = to_color;
	if (isOn && mode == 0) // prevent performance issues when fading while setting color 
		writeColor(color);
}

void LED::setMode(uint8_t to_mode)
{
	mode = to_mode;
}

int LED::getMode()
{
	return mode;
}

RGB mix(RGB color_1, RGB color_2, uint8_t step)
{
	float perc = (float)step/255;

	RGB newCol = {(uint8_t)(perc*color_1.red)	+ (uint8_t)((1-perc)*color_2.red),
		   		  (uint8_t)(perc*color_1.green)	+ (uint8_t)((1-perc)*color_2.green),
		   		  (uint8_t)(perc*color_1.blue)	+ (uint8_t)((1-perc)*color_2.blue)};

	return newCol;
}

RGB fromHSB(uint8_t hue, uint8_t sat, uint8_t val) { 
	/*
	 * Hue: 0..359
	 * Sat: 0..255
	 * Val: 0..255
	 */
	RGB result;
 	val = dim_curve[val];
 	sat = 255-dim_curve[255-sat];
	uint8_t r, b, g, base;

  	if (sat == 0) //Acromatic, hue not needed
  	{ 
    	result.red = val;
	    result.green = val;
	    result.blue = val; 
	} 
	else  
	{ 
		base = ((255 - sat) * val)>>8;

	    switch(hue/60) 
	    { 
		    case 0:// 0..59
		    {
		        result.red = val;		
		        result.green = (((val-base)*hue)/60)+base;		
		        result.blue = base;		
		    	break;
			}

		    case 1://60..119
		    {
		        result.red = (((val-base)*(60-(hue%60)))/60)+base;		
		        result.green = val;		
		        result.blue = base;		
		    	break;
		    }

		    case 2://120..179
		    {
		        result.red = base;		
		        result.green = val;		
		        result.blue = (((val-base)*(hue%60))/60)+base;		
		    	break;
			}

			case 3://180..239
		    {
		        result.red = base;		
		        result.green = (((val-base)*(60-(hue%60)))/60)+base;		
		        result.blue = val;		
		    	break;
		    }

			case 4://240..299
		    {
		        result.red = (((val-base)*(hue%60))/60)+base;		
		        result.green = base;		
		        result.blue = val;		
		    	break;
		    }

			case 5://300..359
		    {
		        result.red = val;		
		        result.green = base;		
		        result.blue = (((val-base)*(60-(hue%60)))/60)+base;		
		    	break;
		    }		

		}
	}   
	return result;
}
