
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <stdarg.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#define DEBUG 1 // debug mode with verbose output over serial at 115200 bps
#define USE_EEPROM // read nodeID and network settings from EEPROM at bootup, overwrites nodeID and MAC.
#define LEDPIN 6
#define KEEPALIVE 0 // keep connections alive with regular polling to node 0
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel leds = Adafruit_NeoPixel(8, LEDPIN, NEO_GRB + NEO_KHZ800);

RF24 radio(A0,10); // CE, CS. CE at pin A0, CSN at pin 10
RF24Network network(radio);


const unsigned long interval = 2000;
byte sweep=0;
byte nodeID = 1; // Unique Node Identifier (2...254) - also the last byte of the IPv4 adress, not used if USE_EEPROM is set
uint16_t this_node = 00; // always begin with 0 for octal declaration
unsigned long iterations=0;
unsigned long errors=0;
unsigned int loss=0;
unsigned long p_sent=0;
unsigned long p_recv=0;
// Variables for the 32bit unsigned long Microsecond rollover handling
static unsigned long microRollovers=0; // variable that permanently holds the number of rollovers since startup
static unsigned long halfwayMicros = 2147483647; // this is halfway to the max unsigned long value of 4294967296
static boolean readyToRoll = false; // tracks whether we've made it halfway to rollover

const short max_active_nodes = 32;
uint16_t active_nodes[max_active_nodes];
short num_active_nodes = 0;
short next_ping_node_index = 0;
unsigned long last_time_sent;
unsigned long updates = 0;
void add_node(uint16_t node);
boolean send_T(uint16_t to);
void send_L1(int to, int _b);
void handle_L(RF24NetworkHeader& header);
void handle_T(RF24NetworkHeader& header);
void handle_B(RF24NetworkHeader& header);
void ledupdate(byte* ledmap);
void p(char *fmt, ... );

void ledst(int sta=127){
  uint32_t c;
  switch (sta) {
  case 0:
    c=leds.Color(0, 0, 0);
    break;
  case 1:
    c=leds.Color(50, 0, 0);
    break;
  case 2:
    c=leds.Color(0, 50, 0);
    break;
  case 3:
    c=leds.Color(0, 0, 50);
    break;
  case 4:
    c=leds.Color(25, 0, 25);
    break;
  case 5:
    c=leds.Color(25, 25, 0);
    break;
  case 6:
    c=leds.Color(255, 0, 0);
    break;
  case 10:
    c=leds.Color(255, 255, 255);
    break;      
  case 11:
    c=leds.Color(255, 0, 0);
    break;
  case 12:
    c=leds.Color(0, 255, 0);
    break;
  case 13:
    c=leds.Color(0, 0, 255);
    break;
  default:
    c=leds.Color(20, 2, 2);
    break;
  }
  leds.setPixelColor(0, c);
  leds.show();
}
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
    ; // wait for serial port to connect. Needed for Leonardo only
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
  Serial.print(", (dec): ");
  Serial.print(this_node,DEC);
  Serial.println();
  SPI.begin(); // SPI for the NRF24
  radio.begin(); // init of the NRF24
  // The amplifier gain can be set to RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_MAX=0dBm.
  radio.setPALevel(RF24_PA_MAX); // transmitter gain value (see above)
  network.begin( 1, this_node ); // fixed radio channel, node ID
  Serial.print(F("UID: "));
  p("%010ld: Starting up\n", millis());
  colorWipe(leds.Color(100, 0, 0), 100); // Red
  colorWipe(leds.Color(0, 100, 0), 100); // Green
  colorWipe(leds.Color(0, 0, 100), 100); // Blue
  colorWipe(leds.Color(0, 0, 0), 0); // clear
  byte ledmap[24]={
    100,100,100,
    100,100,0,
    100,0,100,
    100,50,0,
    100,50,50,
    100,50,50,
    100,50,50,
    0,150,0  };
  ledupdate(ledmap);
  colorWipe(leds.Color(0, 0, 0), 0); // clear
  ledst(1);
  radio.printDetails(); // print NRF config registers. Does not work with NRF24network right now?!
  
}
PROGMEM prog_uchar red[]=
{255,0,0,
255,0,0,
255,0,0,
255,0,0,
255,0,0,
255,0,0,
255,0,0,
255,0,0};
PROGMEM prog_uchar blue[]=
{0,0,255,
0,0,255,
0,0,255,
0,0,255,
0,0,255,
0,0,255,
0,0,255,
0,0,255};
byte pat1[]={10,0,0,
10,10,0,
0,10,0,
0,10,10,
0,0,10,
10,0,10,
10,5,0,
5,16,5};

void loop(void)
{

  //  colorWipe(leds.Color(millis(), 0, millis()/64), 20); // Red
  //  colorWipe(leds.Color(25, 205, 0), 50); // Green
  //  colorWipe(leds.Color(0, 134, 225), 50); // Blue

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
    ledst(2);
    /* // unsigned long int rollover checking:
    Serial.print(microsRollover()); // how many times has the unsigned long micros() wrapped?
     Serial.print(":"); //separator 
     Serial.print(nowM); //micros();
     Serial.print("\n"); //new line
     */
    if (DEBUG) {
      p("%010ld: %ld net updates/s\n",millis(),updates*1000/interval);
    }
    updates = 0;
    last_time_sent = now;
    if (KEEPALIVE) {
    uint16_t to = 000;
    bool ok = 0;
    if ( to != this_node)
    {
      unsigned long nowM = micros();
      ok = send_T(to);
      if (DEBUG) {
      p(" in %ld us.\n", (micros()-nowM) );
      }
      if (ok){
        p_sent++;
      }
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
    sweep+=20;
    if (sweep>254) sweep=0;
    
    if ( this_node == 00){
    for (short _i=0; _i<num_active_nodes; _i++) {
    send_L1(active_nodes[_i],(byte) sweep&0xFF);
    }
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
void send_L1(int to, int _b = 0){
  if ( to != this_node) {      
     /* byte ledmap[24]={
        1,2,3,
        15,(((byte) millis()&0xFF)/10),(((byte) millis()+127&0xFF)/10),
        5,15,5,
        0,5,15,
        15,5,0,
        5,15,5,
        0,5,15,
        5,0,(((byte) millis()&0xFF)/10)};
        
        ledmap[min(23,to*3)]=((byte) millis()&0xFF);
     */
     
     byte ledmap[24]={
        1,2,3,
        0,0,_b,
        0,0,_b,
        0,0,_b,
        0,0,_b,
        0,0,_b,
        0,0,_b,
        0,(((byte) millis()&0xFF)/5),_b};
        
        //ledmap[min(23,to+3)]=((byte) sweep&0xFF);
        //ledmap[min(23,to+4)]=(((byte) sweep+127)&0xFF);
        //ledmap[min(23,to+5)]=(((byte) sweep)&0xFF);
        
/*      Serial.println();
        for(uint16_t i=0; i<sizeof(ledmap); i++) { // print out the packet via serial
        Serial.print(ledmap[i]);
        Serial.print(" ");
      }
      Serial.println();
*/
      unsigned long now = millis();
      bool ok = send_L(to, ledmap);
      if (DEBUG) {
      p(" in %ld ms.\n", (millis()-now) );
      if (ok){
      }
      if (!ok)
      {
        p("%010ld: send_L timout.\n", millis()); // An error occured..
      }
      }
      ledst();
    }
  }
    
boolean send_T(uint16_t to) // Send out this nodes' time -> Timesync!
{
  if (DEBUG) {
  p("%010ld: Sent 'T' to   %05o", millis(),to);
  }
  RF24NetworkHeader header(to,'T');
  unsigned long time = micros();
  return network.write(header,&time,sizeof(time));
}

boolean send_L(uint16_t to, byte* ledmap) // Send out an LED map
{
  if (DEBUG) {
  p("%010ld: Sent 'L' to   %05o", millis(),to);
  }
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
  if (DEBUG) {
  p("%010ld: Recv 'K' from %05o\n", millis(), header.from_node);
  }
  ledupdate(kmap);
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

