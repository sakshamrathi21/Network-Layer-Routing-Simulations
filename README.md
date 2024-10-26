# Routing Protocol for Simulated Network

This repository contains an implementation of a custom **Routing Protocol (RP)** designed to manage packet routing in a simulated network, ensuring quick convergence to the shortest path between nodes.


## File Structure

- **rp.h**: Contains header files and definitions for IP, MAC address types, and function prototypes for RP.
- **RPNode Class**: Manages each node's routing table, packet handling, and broadcast operations.
- **RPHeader Struct**: Manages packet headers and data payloads.

---

## Code Overview

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

Handles packet creation and routing, managing destination lookup and broadcasting to neighbors if the destination is unknown.

```cpp
void RPNode::send_segment(IPAddress dest_ip, const std::vector<uint8_t>& segment);
```

### `RPNode::receive_packet()`

Processes received packets by distinguishing between informational packets and data packets, updating routing tables if necessary.

```cpp
void RPNode::receive_packet(MACAddress src_mac, std::vector<uint8_t> packet, size_t distance);
```

- **Info Packet**: Contains `IP,MAC,DISTANCE,VALIDITY`, used to update the routing table.
- **Data Packet**: Checked for the destination match and forwarded accordingly, respecting TTL values.

### `RPNode::do_periodic()`

Performs periodic routing updates, decrementing route validity counters and sending updated routing information to neighbors.

```cpp
void RPNode::do_periodic();
```

## Usage Instructions

```bash
make -j
./bin/main rp <netspec file> <msgs file> --delay <delay in ms>
```

The delay field is optional, it is set to 50ms by default, this corresponds to the time given to the network to converge after an UP/DOWN/initially.

---

## Constants

- `MAX_TTL`: The maximum time to leave. It is decreased for every hop in the path. The maximum number of nodes is assumed to be 15, so MAX_TTL is said to be 15.
- `COUNTER_VALUE`: This sets the validity period of the entries of the routing table for a node. The max validity is set to be 20. This validity is shared across nodes along with the other routing table fields. Also, the unit of measuring this validity is a single call of do_periodic() function.


## Error Handling

- **Unreachable Routes**: If a route is unreachable, the distance is set to infinity.
- **TTL Expiry**: Packets are dropped upon reaching a TTL of 0, preventing infinite routing.

## Author

**Saksham Rathi**
