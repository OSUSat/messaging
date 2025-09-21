#include "packet.hpp"
#include <cstdint>
#include <stdexcept>

namespace OSUSat {

uint16_t crc16_ccitt(const std::vector<uint8_t> &data) {
    uint16_t crc = 0xFFFF; // initial value

    for (uint8_t byte : data) {
        // move byte left by 8 bits & XOR against current crc
        // effectively moves the byte into the most significant bits of the crc
        // register
        crc ^= static_cast<uint16_t>(byte) << 8;

        for (int i = 0; i < 8; ++i) {
            // is the most significant bit set?
            if (crc & 0x8000) {
                // shift left & XOR with the polynomial
                crc = (crc << 1) ^ 0x1021;
            } else {
                // shift left
                crc <<= 1;
            }
        }
    }

    return crc;
}

Packet::Packet(uint8_t version, OSUSatDestination destination,
               OSUSatDestination source, OSUSatMessageType message_type,
               uint8_t command_id, const std::vector<uint8_t> &payload,
               uint8_t sequence, bool is_last_chunk)
    : version_(version), destination_(destination), source_(source),
      message_type_(message_type), command_id_(command_id), sequence_(sequence),
      is_last_chunk_(is_last_chunk), payload_(payload) {
    if (payload_.size() > MAX_PAYLOAD_SIZE) {
        throw std::runtime_error("Payload size cannot exceed 255 bytes.");
    }
}

std::vector<uint8_t> Packet::pack() const {
    std::vector<uint8_t> buffer;
    buffer.reserve(FRAME_OVERHEAD + payload_.size());

    std::vector<uint8_t> data_for_crc;

    data_for_crc.push_back(version_);
    data_for_crc.push_back(static_cast<uint8_t>(destination_));
    data_for_crc.push_back(static_cast<uint8_t>(source_));
    data_for_crc.push_back(static_cast<uint8_t>(message_type_));
    data_for_crc.push_back(command_id_);
    data_for_crc.push_back(sequence_);
    data_for_crc.push_back(is_last_chunk_ ? 1 : 0);
    data_for_crc.push_back(static_cast<uint8_t>(payload_.size()));
    data_for_crc.insert(data_for_crc.end(), payload_.begin(), payload_.end());

    uint16_t crc = crc16_ccitt(data_for_crc);

    buffer.push_back(START_BYTE);
    buffer.insert(buffer.end(), data_for_crc.begin(), data_for_crc.end());
    buffer.push_back((crc >> 8) & 0xFF); // big endian CRC
    buffer.push_back(crc & 0xFF);

    return buffer;
}

Packet Packet::unpack(const std::vector<uint8_t> &buffer) {
    if (buffer.empty() || buffer.front() != START_BYTE) {
        throw std::runtime_error("Invalid or missing start byte.");
    }

    if (buffer.size() < FRAME_OVERHEAD) {
        throw std::runtime_error("Packet is too short to be valid.");
    }

    std::vector<uint8_t> data_to_check(buffer.begin() + 1,
                                       buffer.end() - CRC_SIZE);

    uint16_t received_crc =
        (static_cast<uint16_t>(buffer[buffer.size() - 2]) << 8) |
        buffer[buffer.size() - 1];

    uint16_t calculated_crc = crc16_ccitt(data_to_check);

    if (received_crc != calculated_crc) {
        throw std::runtime_error(
            "CRC mismatch. Received: " + std::to_string(received_crc) +
            ", Calculated: " + std::to_string(calculated_crc));
    }

    uint8_t payload_len = data_to_check[7];

    if (data_to_check.size() != HEADER_SIZE + payload_len) {
        throw std::runtime_error(
            "Packet actual length does not match length field in header.");
    }

    std::vector<uint8_t> payload(data_to_check.begin() + HEADER_SIZE,
                                 data_to_check.end());

    return Packet(
        data_to_check[0], static_cast<OSUSatDestination>(data_to_check[1]),
        static_cast<OSUSatDestination>(data_to_check[2]),
        static_cast<OSUSatMessageType>(data_to_check[3]), data_to_check[4],
        payload, data_to_check[5], data_to_check[6] != 0);
}

} // namespace OSUSat
