
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <stdarg.h>

RF24 radio(9,8); //CSN at pin 9, CE at pin 8
RF24Network network(radio);

uint16_t this_node = 001; // 001
short node_prime = 79; // 83, 89, 97

const short max_active_nodes = 10;
uint16_t active_nodes[max_active_nodes];
short num_active_nodes = 0;
short next_ping_node_index = 0;
const unsigned long interval = 2000;
unsigned long last_time_sent;
unsigned long updates = 0;
void add_node(uint16_t node);
boolean send_T(uint16_t to);
void handle_T(RF24NetworkHeader& header);
void handle_B(RF24NetworkHeader& header);
void p(char *fmt, ... );

void setup(void)
{
  Serial.begin(115200);
  SPI.begin();
  radio.begin();
  // The amplifier gain can be set to RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  radio.setPALevel(RF24_PA_HIGH); // transmitter gain value (see above)
  network.begin(/*fixed radio channel: */ 117, /*node address: */ this_node );
  p("%ld: Starting up\n", millis());
}

void loop(void)
{
  network.update();
  updates++;
  while ( network.available() ) // while there is some shit filling our pipe
  {
    RF24NetworkHeader header;
    network.peek(header); // preview the header, but don't advance nor flush the packet
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
      p("undefined packet type?");
      break;
    };
  }
  
  unsigned long now = millis();
  if ( now - last_time_sent >= interval ) // non-blocking
  {
    p("%ld estimated updates/s\n",updates*1000/interval);
    updates = 0;
    last_time_sent = now;
    uint16_t to = 00;
    bool ok = 0;
    if ( to != this_node)
    {
      ok = send_T(to);
      if (!ok)
      {
        //last_time_sent -= node_prime; // random awesomeness to stop packets from colliding (at least it tries to)
        p("%ld: I JUST CAN'T DO THIS!\n", millis());
      }
    }
  }   
}
/*
 * T send own time
 * B send back the just received time
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
  if(header.from_node != this_node)
  {
    RF24NetworkHeader header2(header.from_node/*header.from_node*/,'B');
    if(network.write(header2,&time,sizeof(time)))
      p("%ld: send back\n", millis());
  }
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
