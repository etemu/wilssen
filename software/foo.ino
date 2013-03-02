#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

/*
* Send time
* Recv time, ref_t = time - millis()
* work with ref_t + millis() from now
*/

/*

Init as 01, grab an init packet and then change id

*/


RF24 radio(9,10);
RF24Network network(radio);

uint16_t this_node = 001;

const short max_active_nodes = 10;
uint16_t active_nodes[max_active_nodes];
short num_active_nodes = 0;
short next_ping_node_index = 0;

const unsigned long interval = 2000;
unsigned long last_time_sent;

void add_node(uint16_t node);
bool send_H(uint16_t to);
bool send_T(uint16_t to);


void setup(void)
{
  Serial.begin(57600);
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 100, /*node address*/ this_node );
}

void loop(void)
{
  // Pump the network regularly
  network.update();

  // Is there anything ready for us?
  while ( network.available() )
  {
    RF24NetworkHeader header;
    network.peek(header);

    switch (header.type)
    {
    case 'H':
      network.read(header,0,0);
      Serial.print(millis());
      Serial.print(": Recived H from node ");
      Serial.println(header.from_node);
      add_node(header.from_node);
      break;
    default:
      network.read(header,0,0);
      break;
    };
  }
  
  unsigned long now = millis();
  if ( now - last_time_sent >= interval )
  {
    last_time_sent = now;
    uint16_t to = 00;
    
    /*
    bool ok;
    if ( to != this_node)
    {
      ok = send_T(to);
    }  
    // Notify us of the result
    if (!ok)
    {
      last_time_sent -= 100;
    }
    */
  }  
}

bool send_H(uint16_t to)
{
    RF24NetworkHeader header(/*to node*/ to, /*type*/ 'H' /*Time*/);
    return network.write(header,0,0);
}

bool send_T(uint16_t to) // Timesync!
{
    RF24NetworkHeader header(/*to node*/ to, /*type*/ 'T' /*Time*/);
    unsigned long time = millis();
    return network.write(header,time,sizeof(time));
}

void add_node(uint16_t node)
{
  // Do we already know about this node?
  short i = num_active_nodes;
  while (i--)
  {
    if ( active_nodes[i] == node )
      break;
  }
  // If not, add it to the table
  if ( i == -1 && num_active_nodes < max_active_nodes )
  {
    active_nodes[num_active_nodes++] = node; 
    Serial.print(millis());
    Serial.print(": A new node connected -> Node ");
    Serial.println(node);
  }
}
