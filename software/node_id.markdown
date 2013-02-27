# Node-ID

15-bit long, octal system > 5x(0..7), in this case only 5x(0..4) because a node can only have 5 receiving-pipes.

The exit node has the ID 00000, the highest possible node is 55555, which means there can be 5^5=3125 nodes.

* Node 00000 has 5 child-nodes, (00001..00005)  
* Node 00001 has 5 child-nodes, (00011..00051)  
* Node 00011 has 5 child-nodes, (00111..00511) 

and so on.

## parent node

To get a nodes parent node, we need to create a mask for the node_id:

To archive this, we take an "full" octal value and shift it right by one digit (3 bits) until there are only 0s in the mask, where there are non-0 digits in the ID.

Then we need to invert the result to a mask of all non-0 digits.

``` c
uint16_t check = 017777;
uint16_t id = 0321;
while ( id & check )
{
	check <<= 3;
	printf("%o\n", check);
	printf("%o\n", id);
}
```
*note: all numbers are octal*

>177770
>000321
>177700
>000321
>177000
>000321

-> mask = 000777

## parent pipe

To communicate each node needs to know, to which of its parents 5 pipes it is connected to, for once 000321 this would be 3, or the node_id - the parent_id shifted to right by the length of the parent_id:

>parent_id = 21  
>node_id = 321  
>node_id - parent_id = 300  
>parent_id.lenght = 2  
>(node_id - parent_id)>>(parent_id.lenght*3) = 3

or written in C

``` c
foo = parent_id
bar = node_id

while(parent_id)
{
	foo>>=3;
	bar>>=3;
}

print bar
```
