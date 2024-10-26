// Author: Saksham Rathi
// Routing Protocol implemented for the simulated network. Ability to converge under 1ms for graphs of size upto 20ms.
#include "rp.h"
#include <cassert>
#include <iostream>
#include <cstring>
using namespace std;

#define MAX_TTL 15 // Maximum 15 nodes possible in the network
#define COUNTER_VALUE 20 // Counter value for the validity of the information

std::vector<std::string> RPNode::split(const std::string& str, const std::string& delimiter) {
    // This function is used to split the string with the delimiter
    std::vector<std::string> strings_splitted; // The vector to store the splitted strings
    size_t start = 0;
    size_t end = 0;
    while ((end = str.find(delimiter, start)) != std::string::npos) {
        // We need to iterate over the string and split it with the delimiter
        strings_splitted.push_back(str.substr(start, end - start)); 
        start = end + delimiter.length();
    }
    strings_splitted.push_back(str.substr(start)); // We need to add the last string to the vector
    return strings_splitted;
}

struct RPHeader {
    // This is the header for the RP protocol
private:
    RPHeader() = default; // Default constructor
public:
    IPAddress src_ip;   // Source IP
    IPAddress dest_ip;  //  Destination IP
    size_t ttl;         // Time to live

    RPHeader(IPAddress src_ip, IPAddress dest_ip)
        : src_ip(src_ip), dest_ip(dest_ip), ttl(MAX_TTL) { } // Constructor
    static RPHeader from_bytes(uint8_t const* bytes)
    {
        RPHeader ph; // Creating a RPHeader object
        memcpy(&ph, bytes, sizeof(ph)); // Copying the bytes to the object
        return ph;  // Returning the object
    }
};

void RPNode::send_segment(IPAddress dest_ip, std::vector<uint8_t> const& segment) const
{
    RPHeader ph(ip, dest_ip); // Creating a RPHeader object
    std::vector<uint8_t> packet(sizeof(ph) + segment.size()); // Creating a packet
    memcpy(&packet[0], &ph, sizeof(ph));    // Copying the header to the packet
    memcpy(&packet[sizeof(ph)], &segment[0], segment.size()); // Copying the segment to the packet
    // We need to check if the dest_ip is present in our map
    if (ip_to_mac.find(dest_ip) == ip_to_mac.end()) {
        // Not present in the map, so we need to broadcast the packet
        broadcast_packet_to_all_neighbors(packet, true);
    }
    else {
        // Present in the map, so we need to send the packet to the next hop
        if (ip_to_mac.at(dest_ip).first.second == INT32_MAX) {
            // We assume that the algorithm has converged, so a distance of infinity means that the destination is unreachable
            // broadcast_packet_to_all_neighbors(packet, true);
        }
        else 
        // Present in the map, so we need to send the packet to the next hop
        send_packet(ip_to_mac.at(dest_ip).first.first, packet, true);
    }
}

void RPNode::receive_packet(MACAddress src_mac, std::vector<uint8_t> packet, size_t distance)
{
    // Extracting the header from the packet
    RPHeader ph = RPHeader::from_bytes(&packet[0]);
    // We need to check if this is an info regarding the IP to MAC mapping
    // For this we need to get the string out of it (after removing the header)
    std::string s(packet.begin() + sizeof(ph), packet.end());
    // If the packet has "IP,MAC,DISTANCE,," in the start then it is an info packet
    if (s.find("IP,MAC,DISTANCE,VALIDITY,") == 0)
    {
        // We need to extract the information from the packet
        vector<string> v = split(s, ",,"); // Splitting the string with ",,"
        // We need to iterate over the vector and add the information to the map (ignoring the first element as that is the header of the table)
        for (int i = 1; i < (int)v.size(); i++)
        {
            if (v[i].size() == 0) 
                continue; // If the string is empty then no need to do anything
            vector<string> w = split(v[i], ","); // Splitting the string with "," for the information
            // If the IP is not present in the map then we need to add it, else we need to update if the distance is shorter
            if (stoi(w[0]) == (int)ip || stoi(w[1]) == (int)mac) {
                // We need to ignore the information about ourselves, this is basically split horizon to remove count to infinity problem and helps us in avoiding loops and converging faster
                continue;
            }
            if (stoi(w[2]) == INT32_MAX) {
                // Unreachable path information to be handled
                if (ip_to_mac.find(stoi(w[0])) == ip_to_mac.end()) {
                    // Since the distance is infinite, we do not need to add it to the map
                    continue;
                }
                if ((int)ip_to_mac[stoi(w[0])].first.first == stoi(w[1])) {
                    // This information is present in the map and next hop is also same, then we need to update the information in the map
                    ip_to_mac[stoi(w[0])].first.second = INT32_MAX; // Setting the distance to infinity
                    ip_to_mac[stoi(w[0])].second = stoi(w[3]); // validity same as what received, for simultaneous convergence
                }
                continue; // Can skip the lower part
            }
            // We use the same validity information which is received, help us detect down nodes faster
            if (ip_to_mac.find(stoi(w[0])) == ip_to_mac.end()) {
                // If the IP is not present in the map then we need to add it
                ip_to_mac[stoi(w[0])] = { {stoi(w[1]), stoi(w[2])+distance}, stoi(w[3]) };
            }
            else if (ip_to_mac[stoi(w[0])].first.second >= stoi(w[2])+distance) {
                // If the distance is shorter then we need to update the information
                ip_to_mac[stoi(w[0])] = { {stoi(w[1]), stoi(w[2])+distance}, stoi(w[3]) };
            }
            else if ((int)ip_to_mac[stoi(w[0])].first.first == stoi(w[1])) {
                // If the next hop is same then we need to update the validity information
                ip_to_mac[stoi(w[0])].second = stoi(w[3]);
            }
        }
    }
    else {
        // If the packet is not an info packet then it is a data packet
        // We need to check if the packet is for us
        if (ph.dest_ip != ip)
        {
            // If the packet is not for us then we need to check if the TTL is 0
            if (ph.ttl == 0)
                log("Packet dropped");
            else
            {
                // If the TTL is not 0 then we need to decrement the TTL and we need to check if the dest_ip is present in our map
                ph.ttl--;
                if (ip_to_mac.find(ph.dest_ip) == ip_to_mac.end()) {
                    // We have no info about the destination IP, so we need to broadcast the packet
                    broadcast_packet_to_all_neighbors(packet, true);
                }
                else {
                    // We have the information about the destination IP, so we need to send the packet to the next hop
                    send_packet(ip_to_mac[ph.dest_ip].first.first, packet, true);
                }
                    
            }
            return;
        }
        else {
            // If the packet is for us then we need to extract the segment from the packet
            std::vector<uint8_t> segment(packet.begin() + sizeof(ph), packet.end());
            receive_segment(ph.src_ip, segment);
        }
    }
}

void RPNode::do_periodic()
{
    // We need to ensure that the network graph converges to the shortest path
    // We need to add the information about ourselves in the map:
    ip_to_mac[ip] = {{mac, 0}, COUNTER_VALUE};
    // We need to convert the map to a string and send this information to all the neighbors
    std::string s;
    s += "IP,MAC,DISTANCE,VALIDITY,,"; // Upper row of the routing table, will help the receiver to detect that it is information about the routing table
    for (auto &i : ip_to_mac) {
        i.second.second--;
        if (i.second.second == 0) {
            // This means the distance is infinite (validity finished)
            i.second.first.second = INT32_MAX;
        }
        // We will send 4 values: IP, MAC, DISTANCE, VALIDITY
        s += std::to_string(i.first) + "," + std::to_string(mac) + "," + std::to_string(i.second.first.second) + "," + std::to_string(i.second.second) + ",,";
    }
    // Creating a RP Header
    RPHeader ph(ip, 0);
    // Initializing the packet
    std::vector<uint8_t> packet(sizeof(ph) + s.size());
    // Copying the header to the packet
    memcpy(&packet[0], &ph, sizeof(ph));
    // Copying the information to the packet
    memcpy(&packet[sizeof(ph)], &s[0], s.size());
    // Broadcasting the packet to all the neighbors
    broadcast_packet_to_all_neighbors(packet, false);    
}