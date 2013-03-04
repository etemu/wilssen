#include <RF24Helper.h>

void setup() { 
  Serial.begin(9600); 
} 

void loop() { 
  for(int i = 0;i<=100;i++)
  {
    nodeconfig_write(i);
    Serial.println(nodeconfig_read());
    delay(2000);
  }
}
