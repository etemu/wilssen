/* 
 * Not confirmed to be working
 * NEW: Primes and timesync
 */


#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <stdarg.h>

RF24 radio(9,10);
RF24Network network(radio);

uint16_t this_node = 001; // 000;
short node_prime = 79; // 83, 89, 97

const short max_active_nodes = 10;
uint16_t active_nodes[max_active_nodes];
short num_active_nodes = 0;
short next_ping_node_index = 0;
const unsigned long interval = 2000;
unsigned long last_time_sent;

void add_node(uint16_t node);
boolean send_T(uint16_t to);
void handle_T(RF24NetworkHeader& header);
void handle_B(RF24NetworkHeader& header);
void p(char *fmt, ... );

void setup(void)
{
  Serial.begin(57600);
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 100, /*node address*/ this_node );
}

void loop(void)
{
  network.update();
  while ( network.available() ) // while there is some shit filling our pipe
  {
    RF24NetworkHeader header;
    network.peek(header); // preview
    switch (header.type)
    {
    case 'T':
      handle_T(header);
      break;
    case 'B':
      handle_B(header);
      break;      
    default:
      network.read(header,0,0);
      break;
    };
  }
  
  unsigned long now = millis();
  if ( now - last_time_sent >= interval ) // non-blocking
  {
    last_time_sent = now;
    uint16_t to = 00;
    bool ok;
    if ( to != this_node)
      ok = send_T(to);
    if (!ok)
      last_time_sent -= 100; // random awesomeness to stop packets from colliding (at least it tries to)
  }  
}
/*
 * T send own time
 * B send back the recv T
 * P send ping
 */
boolean send_T(uint16_t to) // Timesync!
{
  RF24NetworkHeader header(to,'T');
  unsigned long time = millis();
  return network.write(header,&time,sizeof(time));
}

void handle_T(RF24NetworkHeader& header)
{
  unsigned long time;
  network.read(header,&time,sizeof(time));
  p("%ld: Recv 'T' from node %o -> %ld", millis(), header.from_node, time);
  add_node(header.from_node);
  RF24NetworkHeader header2(header.from_node,'T');
  if(network.write(header2,&time,sizeof(time)))
    p("%ld: send back", millis());
}

void handle_B(RF24NetworkHeader& header)
{
  unsigned long ref_time;
  network.read(header,&ref_time,sizeof(ref_time));
  p("%ld: Recv 'B' from node %o -> %ldms round trip delay", millis(), header.from_node, millis()-ref_time);
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

void add_node(uint16_t node)
{
  short i = num_active_nodes;
  while (i--)
    if ( active_nodes[i] == node ) break; // Do we already know about this node?
  if ( i == -1 && num_active_nodes < max_active_nodes )  // If not and there is enough place, add it to the table
  {
    active_nodes[num_active_nodes++] = node; 
    p("%ld: Added a new node -> %o", millis(), node);
  }
}
