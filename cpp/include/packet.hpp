/**
 * @file packet.hpp
 * @brief OSUSat/SCRT Messaging Standard Packet Library (C++)
 *
 * Provides a class to represent, serialize, and deserialize packets
 */
#ifndef OSUSAT_PACKET_HPP
#define OSUSAT_PACKET_HPP

#include "messages.hpp" // assumes the generated C++ messages.hpp is available
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace OSUSat {

// --- Constants ---
constexpr uint8_t START_BYTE = 0x7E;
constexpr size_t HEADER_SIZE = 8;
constexpr size_t CRC_SIZE = 2;
constexpr size_t FRAME_OVERHEAD = 1 + HEADER_SIZE + CRC_SIZE;
constexpr size_t MAX_PAYLOAD_SIZE = 255;

/**
 * @brief Calculates the CRC-16-CCITT checksum for a data buffer.
 */
uint16_t crc16_ccitt(const std::vector<uint8_t> &data);

class Packet {
  public:
    /**
     * @brief Constructs an OSUSat Packet object.
     */
    Packet(uint8_t version, OSUSatDestination destination,
           OSUSatDestination source, OSUSatMessageType message_type,
           uint8_t command_id, const std::vector<uint8_t> &payload = {},
           uint8_t sequence = 0, bool is_last_chunk = true);

    /**
     * @brief Serializes the Packet object into a byte vector for transmission.
     * @return A std::vector<uint8_t> containing the fully packed message.
     */
    std::vector<uint8_t> pack() const;

    /**
     * @brief Deserializes a byte vector into a Packet object.
     * @param buffer The raw byte vector received from a transmission.
     * @return A constructed Packet object.
     * @throws std::runtime_error if the packet is invalid (bad CRC, size,
     * etc.).
     */
    static Packet unpack(const std::vector<uint8_t> &buffer);

    // --- Getters ---
    uint8_t getVersion() const { return version_; }
    OSUSatDestination getDestination() const { return destination_; }
    OSUSatDestination getSource() const { return source_; }
    OSUSatMessageType getMessageType() const { return message_type_; }
    uint8_t getCommandId() const { return command_id_; }
    uint8_t getSequence() const { return sequence_; }
    bool isLastChunk() const { return is_last_chunk_; }
    const std::vector<uint8_t> &getPayload() const { return payload_; }

  private:
    uint8_t version_;
    OSUSatDestination destination_;
    OSUSatDestination source_;
    OSUSatMessageType message_type_;
    uint8_t command_id_;
    uint8_t sequence_;
    bool is_last_chunk_;
    std::vector<uint8_t> payload_;
};

} // namespace OSUSat

#endif // OSUSAT_PACKET_HPP
