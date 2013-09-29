/*
 * Read out the first 32 EEPROM values.
 *
 */

#include <EEPROM.h>

// start reading from the first byte (address 0) of the EEPROM
int address = 0;
byte value;
int i =42;

void setup()
{
  // initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
}

void loop()
{
  // read a byte from the current address of the EEPROM
  value = EEPROM.read(address);
  Serial.print(address);
  Serial.print("\t");
  Serial.print(value, DEC);
  Serial.println();
  
  // advance to the next address of the EEPROM
  address = address + 1;
  
  if (address >= i){ //do nothing furthermore.
    while(i<1337) {
    delay(42);
  }
  }
  delay(20);
}
