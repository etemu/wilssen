#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

void setup(){
  Serial.begin(9600);
  Mirf.spi = &MirfHardwareSpi;  
  Mirf.init(); 
  Mirf.setRADDR((byte *)"serv1"); 
  Mirf.payload = sizeof(float);   
  Mirf.config();
  Serial.println("Listening..."); 
}

void loop(){

  float temp;
  unsigned long time = millis();
    while(!Mirf.dataReady()){
    //Serial.println("Waiting");
    if ( ( millis() - time ) > 2000 ) {
      Serial.println("Timeout on response from server!");
      return;
    }
  }
  
  Mirf.getData((byte *) &temp);
  Serial.println(temp);
}
