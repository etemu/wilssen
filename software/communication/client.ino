#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <RGBlink.h>

Adafruit_BMP085 bmp;
LED led(11,9,10);

unsigned long prevMillis = 0;

void setup(){
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"clie1");
  Mirf.payload = sizeof(float);
  Mirf.config();
  bmp.begin();
}

void loop(){
  unsigned long currentMillis = millis();
  if(currentMillis - prevMillis >= 1000)
  {
      float temp = bmp.readTemperature();
      unsigned long time = millis();
      Mirf.setTADDR((byte *)"serv1");
      Mirf.send((byte *)&temp);
      while(Mirf.isSending()){}
      led.flash(100);
      prevMillis = currentMillis;
  }
  delay(5);
  led.update();
} 
