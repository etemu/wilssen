#include <RGBlink.h>

//define your RGB-LED like this:
// LED led(red_pin,green_pin,blue_pin);
LED led(11,9,10);

void setup()   {                
}

void loop()                     
{
  //there are various presaved colors like red, green, yellow, blue and white
  led.setColor(red);
  delay(1000);
  led.setColor(yellow);
  delay(1000);
  led.setColor(green);
  delay(1000);
}