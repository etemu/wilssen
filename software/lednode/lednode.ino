
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <stdarg.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#define DEBUG 0 // debug mode with verbose output over serial at 115200 bps
#define USE_EEPROM // read nodeID and network settings from EEPROM at bootup, overwrites nodeID and MAC.
#define LEDPIN 6
#define KEEPALIVE 1 // keep connections alive with regular polling to node 0

Adafruit_NeoPixel leds = Adafruit_NeoPixel(8, LEDPIN, NEO_GRB + NEO_KHZ800); // number of pixels in strip, pin number, pixel type flags

RF24 radio(A0,10); // CE, CS. CE at pin A0, CSN at pin 10
RF24Network network(radio); // mesh network layer 

const unsigned long interval = 30000; // KEEPALIVE interval in [ms]
byte sweep=0;
byte nodeID = 1; // Unique Node Identifier (2...254) - also the last byte of the IPv4 adress, not used if USE_EEPROM is set
uint16_t this_node = 00; // always begin with 0 for octal declaration, not used if USE_EEPROM is set
// Debug variables, TODO: don't initialize if DEBUG is set to 0
unsigned long iterations=0;
unsigned long errors=0;
unsigned int loss=0;
unsigned long p_sent=0;
unsigned long p_recv=0;
// Variables for the 32bit unsigned long Microsecond rollover handling
static unsigned long microRollovers=0; // variable that permanently holds the number of rollovers since startup
static unsigned long halfwayMicros = 2147483647; // this is halfway to the max unsigned long value of 4294967296
static boolean readyToRoll = false; // tracks whether we've made it halfway to rollover

const short max_active_nodes = 32; // the size of the array with active nodes
uint16_t active_nodes[max_active_nodes];
short num_active_nodes = 0;
short next_ping_node_index = 0;
unsigned long last_time_sent;
unsigned long updates = 0; // has to be changed to unsigned long if the interval is too long
void add_node(uint16_t node);
boolean send_T(uint16_t to);
void send_L1(int to, int _b);
void handle_K(RF24NetworkHeader& header);
void handle_L(RF24NetworkHeader& header);
void handle_T(RF24NetworkHeader& header);
void handle_B(RF24NetworkHeader& header);
void ledupdate(byte* ledmap);
void p(char *fmt, ... );
void ledst(int sta=127);

bool strobe=1; 

void setup(void)
{
  pinMode(A6, INPUT); // some nodes have a sense wire to the 3v3 for the NRF24 module via external LDO
  pinMode(A7, INPUT); // see above
  leds.begin(); // the 8 LEDs
  leds.show(); // Initialize all pixels to 'off'
  pinMode(A1, OUTPUT); // GND for the NRF24 module
  digitalWrite(A1, LOW); // GND for the NRF24 module
  pinMode(2, OUTPUT); // Vcc for the NRF24 module, 3.5-5V output to an LDO supplying 3.3V
  digitalWrite(2, HIGH); // Vcc for the NRF24 module activated. Shutdown with LOW.
  Serial.begin(115200);
  delay(128); // wait for the serial interface to boot up
  while (!Serial) {
    ; // wait for serial port to connect.
  }
#ifdef USE_EEPROM
  nodeID=EEPROM.read(0);
  Serial.print(F("EEPROM, "));
#endif
  Serial.println(nodeID);
  Serial.print(F("Network ID (oct): "));
#ifdef USE_EEPROM
  this_node = ((int) EEPROM.read(15)*256) + ((int) (EEPROM.read(16)));
  Serial.print(F("EEPROM, "));
#endif
  Serial.print(this_node,OCT);
  Serial.print(F(", (dec): "));
  Serial.print(this_node,DEC);
  Serial.println();
  SPI.begin(); // SPI for the NRF24
  radio.begin(); // init of the NRF24
  // The amplifier gain can be set to RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_MAX=0dBm.
  radio.setPALevel(RF24_PA_MAX); // transmitter gain value (see above)
  network.begin( 1, this_node ); // fixed radio channel, node ID
  Serial.print(F("UID: \n"));
  p("%010ld: Starting up\n", millis());
  colorWipe(leds.Color(100, 0, 0), 100); // Red
  colorWipe(leds.Color(0, 100, 0), 100); // Green
  colorWipe(leds.Color(0, 0, 100), 100); // Blue
  colorWipe(leds.Color(0, 0, 0), 0); // clear
  ledst(2);
  //  radio.printDetails(); // print NRF config registers. Does not work with NRF24network right now?!
}

void loop(void)
{
  ledst();
  network.update();
  updates++;
  while ( network.available() ) // while there are packets in the FIFO buffer
  {
    ledst(3); // light up status LED with pattern #3
    RF24NetworkHeader header; // initialize header
    network.peek(header); // preview the header, but don't advance nor flush the packet
    switch (header.type) // check which packet type we received
    {
    case 'K':
      handle_K(header);
      break;
    case 'L':
      handle_L(header);
      break;
    case 'T':
      handle_T(header);
      break;
    case 'B':
      handle_B(header);
      break;      
    default:
      network.read(header,0,0); // if none of the above packet types matched, read out and flush the buffer
      if (DEBUG) {
        Serial.print(F("            undefined packet type: ")); // print the unrecognized packet type
        Serial.print(header.type);
        Serial.println();
      }
      break;
    };
    ledst(); // reset the status LED to the default pattern
  }
  unsigned long now = millis();
  unsigned long nowM = micros();
  if ( now - last_time_sent >= interval ) // non-blocking check for start of debug service routine interval
  {
    ledst(1); //red
    /* // unsigned long int rollover checking:
     Serial.print(microsRollover()); // how many times has the unsigned long micros() wrapped?
     Serial.print(":"); //separator 
     Serial.print(nowM); //micros();
     Serial.print("\n"); //new line
     */
    if (DEBUG) {
      p("%010ld: %ld Hz\n",millis(),updates*1000/interval);
    }
    updates = 0;
    last_time_sent = now;
    if (KEEPALIVE) {
      uint16_t to = 00;
      bool ok = 0;
      if ( to != this_node)
      {
        unsigned long nowM = micros();
        ok = send_T(to);
        if (DEBUG) p(" in %ld us.\n", (micros()-nowM) );
        if (ok) p_sent++;
        if (!ok)
        {
          errors++;
          if (DEBUG) {
            p("%010ld: No ACK timeout.\n", millis()); // An error occured, need to stahp!
          }
        }
        iterations++;
        /*
      Serial.print("loop: \t\t");
         Serial.println(iterations);
         Serial.print("errors: \t\t");
         Serial.println(errors);     
         Serial.print("send error in %: \t");
         Serial.println(errors*100/iterations);
         Serial.print("pkts sent    : \t");
         Serial.println(p_sent);
         Serial.print("pkts received: \t");
         Serial.println(p_recv);
         Serial.print("replies in %: \t");
         Serial.println(p_recv*100/(p_sent-1));
         */
      }
    }
   /*
    if ( this_node == 00){
      for (short _i=0; _i<num_active_nodes; _i++) {
        send_L1(active_nodes[_i],(byte) sweep&0xFF);
      }
      strobe=!strobe;
    }
    */
    if ( this_node == 00){
      send_L1(01,(byte) sweep&0xFF);
      //send_L1(00,(byte) sweep&0xFF);
      strobe=!strobe;
    }
  }
}
/* WIP DRAFT, TBD
 C voltage (1-24 byte) fixed point values
 D current (1-24 byte) fixed point values
 E battery voltage (2 byte)
 F error code + error value
 G 
 K draft for home automation packet
 L LED map
 T send out timestamp
 V software version, UID, wID, location
 B reply with the just received timestamp (T->B)
 */
 
void send_K(int to){
   byte ledmap[24]={
      1,2,3,
      0,0,1,
      0,0,1,
      0,0,1,
      0,0,1,
      0,0,1,
      0,0,1,
      0,(((byte) millis()&0xFF)/5),1        };

    unsigned long now = millis();
    bool ok = send_L(to, ledmap);
    if (DEBUG) {
      p(" in %ld ms.\n", (millis()-now) );
      if (ok){}
      if (!ok) p("%010ld: send_K timout.\n", millis()); // An error occured..
    }
  }
 
void send_L1(int to, int _b = 0){
  if ( to != this_node) {      
    byte ledmap[24]={
      1,2,3,
      0,0,_b,
      0,0,_b,
      0,0,_b,
      0,0,_b,
      0,0,_b,
      0,0,_b,
      0,(((byte) millis()&0xFF)/5),_b        };

    unsigned long now = millis();
    bool ok = send_L(to, ledmap);
    if (DEBUG) {
      p(" in %ld ms.\n", (millis()-now) );
      if (ok){}
      if (!ok) p("%010ld: send_L timout.\n", millis()); // An error occured..
    }
    ledst();
  }
}

boolean send_T(uint16_t to) // Send out this nodes' time -> Timesync!
{
  if (DEBUG) p("%010ld: Sent 'T' to   %05o", millis(),to);
  RF24NetworkHeader header(to,'T');
  unsigned long time = micros();
  return network.write(header,&time,sizeof(time));
}

boolean send_L(uint16_t to, byte* ledmap) // Send out an LED map
{
  if (DEBUG) p("%010ld: Sent 'L' to   %05o", millis(),to);
  RF24NetworkHeader header(to,'L');
  return network.write(header,ledmap,24);
}

void ledupdate(byte* ledmap){
  for(uint8_t i=0; i<leds.numPixels(); i++) {
    uint32_t c = leds.Color(ledmap[i*3],ledmap[(i*3)+1],ledmap[(i*3)+2]);
    leds.setPixelColor(i, c);
  }
  leds.show();
}

void handle_K(RF24NetworkHeader& header)
{
  byte kmap[24];
  network.read(header,kmap,sizeof(kmap));
  if (DEBUG) p("%010ld: Recv 'K' from %05o\n", millis(), header.from_node);
  byte ledmap[24]={
    000,000,000, // status LED at 0
    kmap[0],kmap[1],kmap[2], // acc values
    kmap[3],kmap[3],kmap[3], // gyro x
    kmap[4],kmap[4],kmap[4], // gyro y...
    kmap[5],kmap[5],kmap[5],    
    000,000,00,
    000,000,00,
    000,000,0      };
  ledupdate(ledmap);

  if (DEBUG) {
    for(uint16_t i=0; i<sizeof(kmap); i++) { // print out the received packet via serial
      Serial.print(kmap[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void handle_L(RF24NetworkHeader& header)
{
  byte ledmap[24];
  network.read(header,ledmap,sizeof(ledmap));
  if (DEBUG) {
    p("%010ld: Recv 'L' from %05o\n", millis(), header.from_node);
  }
  ledupdate(ledmap);
  if (DEBUG) {
    for(uint16_t i=0; i<sizeof(ledmap); i++) { // print out the received packet via serial
      Serial.print(ledmap[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void handle_T(RF24NetworkHeader& header)
{
  unsigned long time;
  network.read(header,&time,sizeof(time));
  if (DEBUG) {
    p("%010ld: Recv 'T' from %05o:%010ld\n", millis(), header.from_node, time);
  }
  add_node(header.from_node);  
  if(header.from_node != this_node)
  {
    RF24NetworkHeader header2(header.from_node/*header.from_node*/,'B');
    unsigned long nowM = micros();
    if(network.write(header2,&time,sizeof(time)))
      if (DEBUG) {
        p("%010ld: Answ 'B' to   %05o in ", millis(),header.from_node);
        Serial.print(micros()-nowM-16);
        Serial.print(F(" us.\n"));
      }
  }
}

void handle_B(RF24NetworkHeader& header)
{
  p_recv++;
  unsigned long ref_time;
  network.read(header,&ref_time,sizeof(ref_time));
  if (DEBUG) {
    p("%010ld: Recv 'B' from %05o -> %ldus round trip\n", millis(), header.from_node, micros()-ref_time);
  }
}

// Arduino version of the printf()-funcition in C 
void p(char *fmt, ... ){
  char tmp[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt );
  vsnprintf(tmp, 128, fmt, args);
  va_end (args);
  Serial.print(tmp);
}

void add_node(uint16_t node) //TODO: remove_node, after a certain timeout...
{
  short i = num_active_nodes;
  while (i--)
    if ( active_nodes[i] == node ) break; // Do we already know about this node?
  if ( i == -1 && num_active_nodes < max_active_nodes )  // If not and there is enough place, add it to the table
  {
    active_nodes[num_active_nodes++] = node; 
    if (DEBUG) {
      p("%010ld: New node: %05o\n", millis(), node);
    }
  }
}

unsigned long microsRollover() { //based on Rob Faludi's (rob.faludi.com) milli wrapper

    // This would work even if the function were only run once every 35 minutes, though typically,
  // the function should be called as frequently as possible to capture the actual moment of rollover.
  // The rollover counter is good for over 584000 years of runtime. 
  //  --Alex Shure

  unsigned long nowMicros = micros(); // the time right now

  if (nowMicros > halfwayMicros) { // as long as the value is greater than halfway to the max
    readyToRoll = true; // we are in the second half of the current period and ready to roll over
  }

  if (readyToRoll == true && nowMicros < halfwayMicros) {
    // if we've previously made it to halfway
    // and the current millis() value is now _less_ than the halfway mark
    // then we have rolled over
    microRollovers++; // add one to the count of rollovers (approx 71 minutes)
    readyToRoll = false; // we're no longer past halfway, reset!
  } 
  return microRollovers;
}

void colorWipe(uint32_t c, uint8_t wait) { //this is blocking with the hardcoded delay...
  for(uint16_t i=0; i<leds.numPixels(); i++) {
    leds.setPixelColor(i, c);
    leds.show();
    delay(wait);
  }
}



