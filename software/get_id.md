# Automatic ID-finding for new nodes

* Open 5 reading pipes to random nodes

* Wait for a Node-list-packet

* guess the own position based on the pipe the packet came in

* reinit the connection /w our new ID

* send out an updated node-list

Depending on the node-density in the range of our node this will need a few tries.
