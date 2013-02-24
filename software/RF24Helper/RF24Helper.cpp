#include "RF24Helper.h"

uint8_t* address_at_eeprom_location = (uint8_t*)10;
const uint8_t valid_eeprom_flag = 0xdf;

uint8_t nodeconfig_read()
{
  uint8_t result = 0;
  if ( eeprom_read_byte(address_at_eeprom_location) == valid_eeprom_flag )
  {
    result = eeprom_read_byte(address_at_eeprom_location+1);
  }
  return result;
}

void nodeconfig_write(uint8_t val)
{
  eeprom_write_byte(address_at_eeprom_location,valid_eeprom_flag);  
  eeprom_write_byte(address_at_eeprom_location+1,val);
}