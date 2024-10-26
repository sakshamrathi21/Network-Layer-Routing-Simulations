#ifndef RP_H
#define RP_H
#include <unordered_map>

#include "../node.h"

#include <vector>
using namespace std;

class RPNode : public Node {
    // We need to create a mapping between the IP address and the next hop MAC address and the distance to that MAC address. Also each table entry should have a validity value
    std::unordered_map<IPAddress, pair<pair<MACAddress, size_t>, int>> ip_to_mac;

public:
    /*
     * NOTE You may not modify the constructor of this class
     */
    RPNode(Simulation* simul, MACAddress mac, IPAddress ip) : Node(simul, mac, ip) { }

    void send_segment(IPAddress dest_ip, std::vector<uint8_t> const& segment) const override;
    void receive_packet(MACAddress src_mac, std::vector<uint8_t> packet, size_t distance) override;
    void do_periodic() override;
    std::vector<std::string> split(const std::string& str, const std::string& delimiter); // Function to split the string based on the delimiter
};

#endif
