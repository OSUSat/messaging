#include "packet.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/**
 * @brief Calculates the CRC-16-CCITT for a block of data.
 *
 * This implementation is a standard bit-wise calculation and does not require a
 * lookup table, saving space in memory-constrained systems. Polynomial: 0x1021
 */
uint16_t osusat_crc16_ccitt(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF; // initial value

    for (size_t i = 0; i < length; i++) {
        // move byte left by 8 bits & XOR against current crc
        // effectively moves the byte into the most significant bits of the crc
        // register
        crc ^= (uint16_t)data[i] << 8;

        for (int j = 0; j < 8; j++) {
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

int16_t osusat_packet_pack(const OSUSatPacket *packet, uint8_t *buffer,
                           size_t buffer_size) {
    if (packet == NULL || buffer == NULL) {
        return OSUSAT_PACKET_ERR_NULL_PTR;
    }

    size_t total_size = OSUSAT_FRAME_OVERHEAD + packet->payload_len;

    if (buffer_size < total_size) {
        return OSUSAT_PACKET_ERR_BUFFER_TOO_SMALL;
    }

    buffer[0] = OSUSAT_START_BYTE;
    size_t offset = 1;

    buffer[offset++] = packet->version;
    buffer[offset++] = packet->destination;
    buffer[offset++] = packet->source;
    buffer[offset++] = packet->message_type;
    buffer[offset++] = packet->command_id;
    buffer[offset++] = packet->sequence;
    buffer[offset++] = packet->is_last_chunk ? 1 : 0;
    buffer[offset++] = packet->payload_len;

    if (packet->payload_len > 0) {
        memcpy(&buffer[offset], packet->payload, packet->payload_len);
        offset += packet->payload_len;
    }

    uint16_t crc = osusat_crc16_ccitt(&buffer[1],
                                      OSUSAT_HEADER_SIZE + packet->payload_len);

    buffer[offset++] = (crc >> 8) & 0xFF; // big endian CRC
    buffer[offset++] = crc & 0xFF;

    return total_size;
}

OSUSatPacketResult osusat_packet_unpack(OSUSatPacket *packet, uint8_t *buffer,
                                        size_t buffer_size) {
    if (packet == NULL || buffer == NULL) {
        return OSUSAT_PACKET_ERR_NULL_PTR;
    }

    if (buffer_size < OSUSAT_FRAME_OVERHEAD) {
        return OSUSAT_PACKET_ERR_BUFFER_TOO_SMALL;
    }

    if (buffer[0] != OSUSAT_START_BYTE) {
        return OSUSAT_PACKET_ERR_BAD_START_BYTE;
    }

    uint8_t payload_len = buffer[1 + 7]; // length is the 8th byte of the header
    size_t total_packet_size = OSUSAT_FRAME_OVERHEAD + payload_len;

    if (buffer_size < total_packet_size) {
        return OSUSAT_PACKET_ERR_BAD_LENGTH;
    }

    const uint8_t *data_to_check = &buffer[1];

    size_t data_len = OSUSAT_HEADER_SIZE + payload_len;
    uint16_t calculated_crc = osusat_crc16_ccitt(data_to_check, data_len);

    uint16_t received_crc =
        ((uint16_t)buffer[1 + data_len] << 8) | buffer[1 + data_len + 1];

    if (calculated_crc != received_crc) {
        return OSUSAT_PACKET_ERR_BAD_CRC;
    }

    size_t offset = 1;

    packet->version = buffer[offset++];
    packet->destination = buffer[offset++];
    packet->source = buffer[offset++];
    packet->message_type = buffer[offset++];
    packet->command_id = buffer[offset++];
    packet->sequence = buffer[offset++];
    packet->is_last_chunk = (buffer[offset++] != 0);
    packet->payload_len = buffer[offset++];

    if (packet->payload_len > 0) {
        packet->payload = &buffer[offset];
    } else {
        packet->payload = NULL;
    }

    return OSUSAT_PACKET_OK;
}
