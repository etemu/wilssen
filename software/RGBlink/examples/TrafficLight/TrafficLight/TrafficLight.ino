#include <RGBlink.h>

LED led1(11,9,10);

void setup()   {                
}

void loop()                     
{
  led1.setColor(red);
  delay(1000);
  led1.setColor(yellow);
  delay(1000);
  led1.setColor(green);
  delay(1000);
}
