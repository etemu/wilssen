# Nodes (N-Packet)

Sent by the 000-Node, containing all 'active' nodes (id, ping)
When recvived, add new nodes to own node-list


# Time (T-packet)

When recived and sent by a node which is not in the reciving nodes list of active nodes, add it to the list.
(send a N-packet?), if recived a new node send it back to it and wait for a ping-packet.

# Ping (P-packet)

(Do fancy foo to determine the hop-delay between nodes)

# values (V-packet)

Send sensor values and time, when recived by a non-000 node, forward it to the reciving nodes parent node.
When recived by the 'exit-node' write it to a logfile/cosm


# LED-Pattern (L-packet)

Dent by the 000-node, when recived, forward it to all child-nodes, delay (until every node recived it) and blink the pattern.

