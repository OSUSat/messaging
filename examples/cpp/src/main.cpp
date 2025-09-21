#include "messages.hpp"
#include "packet.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

void print_buffer(const std::vector<uint8_t> &buffer) {
    for (uint8_t byte : buffer) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(byte) << " ";
    }

    std::cout << std::dec << std::endl;
}

int main() {
    std::vector<uint8_t> payload = {0xDE, 0xAD, 0xBE, 0xEF};
    OSUSat::Packet packet_to_send(
        1,                                  // version
        OSUSat::OSUSatDestination::PAYLOAD, // destination
        OSUSat::OSUSatDestination::OBC,     // source
        OSUSat::OSUSatMessageType::COMMAND, // message type
        static_cast<uint8_t>(OSUSat::OSUSatPAYLOADCommand::TRANSFER_IMAGE),
        payload);

    std::vector<uint8_t> tx_buffer = packet_to_send.pack();
    std::cout << "Packed " << tx_buffer.size() << " bytes: ";
    print_buffer(tx_buffer);

    try {
        OSUSat::Packet received_packet = OSUSat::Packet::unpack(tx_buffer);

        std::cout << "Successfully unpacked packet." << std::endl;

        std::cout << "  - Destination: 0x" << std::hex
                  << static_cast<int>(received_packet.getDestination())
                  << std::endl;

        std::cout << "  - Command ID:  0x"
                  << static_cast<int>(received_packet.getCommandId())
                  << std::endl;

        std::cout << "  - Payload:     ";

        print_buffer(received_packet.getPayload());

    } catch (const std::runtime_error &e) {
        std::cerr << "Error unpacking packet: " << e.what() << std::endl;
    }

    tx_buffer[5] = 0xFF; // corrupt a byte in the header
    try {
        OSUSat::Packet::unpack(tx_buffer);
    } catch (const std::runtime_error &e) {
        std::cerr << "Caught expected error: " << e.what() << std::endl;
    }

    return 0;
}
