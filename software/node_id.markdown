jekyll-boilerplate
==================
by Leon (l3kn) Rische 

Version 1.0.0

## Usage

###1. Install the following Gems
  
`gem install jekyll`  
`gem install sass`  
`gem install bourbon`  
`gem install jekyll-asset-pipeline`

###2. Change the _config.yaml

###3. run `jekyll`  

##ToDo

* fix template
* documentation



# Node-ID

15-bit long, octal system > 5x(0..7), in this case only 5x(0..4) because a node can only have 5 receiving-pipes.

The exit node has the ID 00000, the highest possible node is 55555, which means there can be 5^5=3125 nodes.

* Node 00000 has 5 child-nodes, (00001..00005)  
* Node 00001 has 5 child-nodes, (00011..00051)  
* Node 00011 has 5 child-nodes, (00111..00511) 

and so on.

To get a nodes parent node, we need to create a mask for the node_id:

To archive this, we take an "full" octal value and shift it right by one digit (3 bits) until there are only 0s in the mask, where there are non-0 digits in the ID.

Then we need to invert the result to a mask of all non-0 digits.

´´´ c
uint16_t check = 017777;
uint16_t id = 0321;
while ( id & check )
{
  check <<= 3;
	printf("%o\n", check);
	printf("%o\n", id);
}
´´´
*note: all numbers are octal*

>177770
>000321
>---
>177700
>000321
>---
>177000
>000321

-> mask = 000777
