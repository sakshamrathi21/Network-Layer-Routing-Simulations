# Routing Protocol for Simulated Network

This repository contains an implementation of a custom **Routing Protocol (RP)** designed to manage packet routing in a simulated network, ensuring quick convergence to the shortest path between nodes.


## File Structure

- **rp.h**: Contains header files and definitions for IP, MAC address types, and function prototypes for RP.
- **rp.c**: Contains all the function bodies.
---

## Code Overview

Although the code is well commented, and the variable names used are self-explanatory, but I have defined the purpose of all the functions which are present in `rp.c`:

### `RPNode::split()`
Splits a string by a specified delimiter and returns the resulting substrings.

```cpp
std::vector<std::string> RPNode::split(const std::string& str, const std::string& delimiter);
```

### `RPHeader Struct`

Defines the packet header structure, holding source/destination IPs, and Time-To-Live (TTL) information.

- **src_ip**: Source IP address.
- **dest_ip**: Destination IP address.
- **ttl**: Time-to-Live (default: `MAX_TTL`).

### `RPNode::send_segment()`

Creates a packet header containing the relevant information. If the destination ip is not in our table, we broadcast it to all neighbours. If the distance is set to be infinite, then we do not do anything (assuming that the algorithm was able to converge in the delay time given). Else, we just send that packet to the next_hop (which is saved from our routing table).

```cpp
void RPNode::send_segment(IPAddress dest_ip, const std::vector<uint8_t>& segment);
```

### `RPNode::receive_packet()`

This function is for receiving packets. There can be two types of packets: data packets or packets with routing table information. Packets with routing information (detected using the header) from neighbours are used to update self routing table. There can be various cases in this, firstly if the packet shared has distance set to `INT_MAX`, then we check if it is the same path which is saved in our table, if yes, then we update our table too (and use the validity from neighbour). This basically signals that one node has got down during the meantime. If the distance is finite, we check if it is a new path. If yes, then we update (in case the distance is smaller). If it is the same path, then we check if it is the same path (if yes, then only we update). The data packets if not meant to us (detected using dst_ip), then we forward it based on our routing table.

```cpp
void RPNode::receive_packet(MACAddress src_mac, std::vector<uint8_t> packet, size_t distance);
```

- **Info Packet**: Contains `IP,MAC,DISTANCE,VALIDITY`, used to update the routing table.
- **Data Packet**: Checked for the destination match and forwarded accordingly, respecting TTL values.

### `RPNode::do_periodic()`

This function is called periodically as the name suggests. In every call it decreases the validity fields of the routing table by 1, collects all the routing table information in a single string and broadcasts that string to all neighbours.

```cpp
void RPNode::do_periodic();
```

## Usage Instructions

```bash
make -j
./bin/main rp <netspec file> <msgs file> --delay <delay in ms>
```

The delay field is optional, it is set to 50ms by default, this corresponds to the time given to the network to converge after an UP/DOWN/initially. `rp` stands for routing protocol which I have implemented. Instead of this you can also use `blaster` and `naive`, which are simpler implementations (these two might not work for all testcases).

---

## Constants

- `MAX_TTL`: The maximum time to leave. It is decreased for every hop in the path. The maximum number of nodes is assumed to be 15, so MAX_TTL is said to be 15.
- `COUNTER_VALUE`: This sets the validity period of the entries of the routing table for a node. The max validity is set to be 20. This validity is shared across nodes along with the other routing table fields. Also, the unit of measuring this validity is a single call of do_periodic() function.


## Additional Features

- The algorithm converges under `1ms` for all types of graphs with at most 20 nodes.
- For a quick convergence, the entire routing table is shared between nodes. `Validity` field is shared too. This helps the neighbours to get information about a down node as soon as possible (starting the validity counter again from max_value will slow down this process).
- `Split Horizon` is used to solve count to infinity information. Basically, when we receive routing table from our neighbours, we do not check those fields for which the next_hop are ourselves (that path goes through us, so we will better know about that path). This again increases the rate of convergence of the algorithm.



## Author

**Saksham Rathi**
