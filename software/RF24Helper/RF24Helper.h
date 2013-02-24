#ifndef __RF24Helper_H__
#define __RF24Helper_H__

#if defined(ARDUINO) && ARDUINO >= 100 //check if using Arduino or Wiring
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif
#include "avr/eeprom.h"
#include "avr/pgmspace.h"

uint8_t nodeconfig_read();
void nodeconfig_write(uint8_t val);

#endif