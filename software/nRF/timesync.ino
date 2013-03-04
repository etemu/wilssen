/* 
 * STATUS: Working (kind of)
 * NEW: random bug, node 000 needs up to 500ms to send back a T-packet(?)
 *      when one node is blocked (wrap hand around nRF-chip) the bug disappears
 *
 * HOWTO:
 * 1. Upload it to one Arduino
 * 2. change node_id to 001
 * 3. Upload it to the second Arduino
 * 4. Serial monitor at 57600Baud
 * 5. ???
 * 6. Profit
 */


#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <stdarg.h>

RF24 radio(9,10);
RF24Network network(radio);

uint16_t this_node = 000; // 000;
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
  network.begin(/*channel*/ 42, /*node address*/ this_node );
  p("%ld: Starting up\n", millis());
}

void loop(void)
{
  network.update();
  while ( network.available() ) // while there is some shit filling our pipe
  {
    p("%ld: Reciving maybe?\n", millis());
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
    bool ok = 0;
    if ( to != this_node)
    {
      p("%ld: Sending maybe?\n", millis());
      ok = send_T(to);
      if (!ok)
      {
        last_time_sent -= node_prime; // random awesomeness to stop packets from colliding (at least it tries to)
        p("%ld: I JUST CAN'T DO THIS!\n", millis());
      }
    }
  }   
}
/*
 * T send own time
 * B send back the recv T
 * P send ping // not yet implemented
 */
boolean send_T(uint16_t to) // Timesync!
{
  p("%ld: Sent time\n", millis());
  RF24NetworkHeader header(to,'T');
  unsigned long time = millis();
  return network.write(header,&time,sizeof(time));
}

void handle_T(RF24NetworkHeader& header)
{
  unsigned long time;
  network.read(header,&time,sizeof(time));
  p("%ld: Recv 'T' from node %o -> %ld\n", millis(), header.from_node, time);
  add_node(header.from_node);  
  RF24NetworkHeader header2(header.from_node,'B');
  if(network.write(header2,&time,sizeof(time)))
    p("%ld: send back\n", millis());
}

void handle_B(RF24NetworkHeader& header)
{
  unsigned long ref_time;
  network.read(header,&ref_time,sizeof(ref_time));
  p("%ld: Recv 'B' from node %o -> %ldms round trip delay\n", millis(), header.from_node, millis()-ref_time);
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
    p("%ld: Added a new node -> %o\n", millis(), node);
  }
}
