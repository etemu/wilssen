/*
 * 
 * Stores the nodeID, ethernet MAC and IP to the EEPROM.
 * These values will stay in the EEPROM when the board is
 * turned off and may be retrieved later when booting the node.
 */

#include <EEPROM.h>

// Change these values according to your network setup:
static int wID = 011; // Wireless mesh networt ID, octal value from 00 to 05555
static byte wirelessID[] = {(wID >> 8) & 0xFF, wID & 0xFF}; //TODO: verify endianess
static byte nodeID = wirelessID[1]; // range: 2-254
static byte mac[] = { 0xAE, 0xAD, 0x42, 0x13, 0x37, nodeID }; // choose a unique MAC
static byte ip[] = { 10, 0, 0, nodeID }; // your subnet IP scheme
static byte gw[] = {10, 0, 0, 254}; // your gateway


byte val[] = {nodeID, mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],ip[0],ip[1],ip[2],ip[3],gw[0],gw[1],gw[2],gw[3],wirelessID[0],wirelessID[1]};

int addr=0;
void setup()
{
  SaveData();
}

void loop()
{
  delay(1337);
  
}

void SaveData(){
  while (addr < sizeof(val)){
  EEPROM.write(addr, val[addr]);
  // advance to the next address. 
  addr++;
    }
}
