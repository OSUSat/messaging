/**
 * @file packet.h
 * @brief OSUSat/SCRT Messaging Standard Packet Library
 *
 * Provides structures and functions to serialize and deserialize packets.
 * This library is designed to be portable and avoid dynamic memory allocation.
 */
#ifndef OSUSAT_PACKET_H
#define OSUSAT_PACKET_H

#include "messages.h" // assumes generated messages.h is available
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// --- Constants ---
#define OSUSAT_START_BYTE 0x7E
#define OSUSAT_MAX_PAYLOAD_LEN 255
#define OSUSAT_HEADER_SIZE 8 // size of the header fields before payload
#define OSUSAT_CRC_SIZE 2

// the total size of the non-payload parts of the packet
#define OSUSAT_FRAME_OVERHEAD (1 + OSUSAT_HEADER_SIZE + OSUSAT_CRC_SIZE)

// --- Structures ---

/**
 * @brief Represents a deserialized OSUSat packet.
 *
 * The payload field points to the data within the original buffer;
 * it is not a separate, allocated copy.
 */
typedef struct {
    uint8_t version;
    OSUSatDestination destination;
    OSUSatDestination source;
    OSUSatMessageType message_type;
    uint8_t command_id;
    uint8_t sequence;
    bool is_last_chunk;
    uint8_t payload_len;
    uint8_t
        *payload; // pointer to the start of the payload in the source buffer
} OSUSatPacket;

// --- Error Codes ---

typedef enum {
    OSUSAT_PACKET_OK = 0,
    OSUSAT_PACKET_ERR_NULL_PTR = -1,
    OSUSAT_PACKET_ERR_BUFFER_TOO_SMALL = -2,
    OSUSAT_PACKET_ERR_BAD_START_BYTE = -3,
    OSUSAT_PACKET_ERR_BAD_CRC = -4,
    OSUSAT_PACKET_ERR_BAD_LENGTH = -5
} OSUSatPacketResult;

// --- Public Functions ---

/**
 * @brief Calculates the CRC-16-CCITT checksum.
 *
 * @param data Pointer to the data buffer.
 * @param length The number of bytes to process.
 * @return The 16-bit CRC value.
 */
uint16_t osusat_crc16_ccitt(const uint8_t *data, size_t length);

/**
 * @brief Serializes an OSUSatPacket struct into a byte buffer.
 *
 * @param packet Pointer to the OSUSatPacket struct to pack.
 * @param buffer Pointer to the output buffer where the packed data will be
 * written.
 * @param buffer_size The total size of the output buffer.
 * @return The total number of bytes written to the buffer, or a negative
 * OSUSatPacketResult on error.
 */
int16_t osusat_packet_pack(const OSUSatPacket *packet, uint8_t *buffer,
                           size_t buffer_size);

/**
 * @brief Deserializes a byte buffer into an OSUSatPacket struct.
 *
 * Note: The `packet->payload` will point into the original `buffer`. No data is
 * copied.
 *
 * @param packet Pointer to the OSUSatPacket struct to populate.
 * @param buffer Pointer to the input buffer containing the raw packet data.
 * @param buffer_size The size of the data in the input buffer.
 * @return OSUSAT_PACKET_OK on success, or a negative OSUSatPacketResult on
 * error.
 */
OSUSatPacketResult osusat_packet_unpack(OSUSatPacket *packet, uint8_t *buffer,
                                        size_t buffer_size);

#endif // OSUSAT_PACKET_H
