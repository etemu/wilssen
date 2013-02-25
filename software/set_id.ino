#include <RGBlink.h>

LED led(11,9,10);
const int buttonPin = 2; 
int buttonState = 0;     
long hightime = 0;
bool lasthigh = 0;
int id = 000;
void setup()
{ 
  pinMode(buttonPin, INPUT);     
  Serial.begin(9600);
  led.off();
}

void loop(){
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH && !lasthigh) 
  {
    hightime = millis();
    lasthigh = 1; 
  } 
  else if (buttonState == LOW){
    if(lasthigh)
    {
      int diff = millis() - hightime;
      if(diff <= 500)
      {
        led.setColor(red);
        led.flash(300);
        id += 1;
      }
      else if(diff <= 1000)
      {
        led.setColor(green);
        led.flash(300);
        id += 10;   
      }
      else if(diff <= 2000)  
        id = 0;
      Serial.println(id);
    }
    hightime = 0;
    lasthigh = 0;
  }
  led.update();
  delay(10);
}
