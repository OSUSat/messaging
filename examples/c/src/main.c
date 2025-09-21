#include "messages.h"
#include "packet.h"
#include <assert.h>
#include <stdio.h>

void print_packet(const OSUSatPacket *p) {
    printf("--- Decoded Packet ---\n");
    printf("  Version:    %u\n", p->version);
    printf("  Destination: 0x%02X\n", p->destination);
    printf("  Source:      0x%02X\n", p->source);
    printf("  Msg Type:    0x%02X\n", p->message_type);
    printf("  Command ID:  0x%02X\n", p->command_id);
    printf("  Payload Len: %u\n", p->payload_len);
    printf("  Payload:     ");

    for (int i = 0; i < p->payload_len; i++) {
        printf("0x%02x ", p->payload[i]);
    }

    printf("\n----------------------\n");
}

int main() {
    uint8_t cmd_payload[] = {0xDE, 0xAD, 0xBE, 0xEF};
    OSUSatPacket packet_to_send = {.version = 1,
                                   .destination = OSUSatDestination_PAYLOAD,
                                   .source = OSUSatDestination_OBC,
                                   .message_type = OSUSatMessageType_COMMAND,
                                   .command_id =
                                       OSUSatPAYLOADCommand_TRANSFER_IMAGE,
                                   .sequence = 42,
                                   .is_last_chunk = true,
                                   .payload_len = sizeof(cmd_payload),
                                   .payload = cmd_payload};

    uint8_t tx_buffer[256];
    int16_t packed_size =
        osusat_packet_pack(&packet_to_send, tx_buffer, sizeof(tx_buffer));

    if (packed_size < 0) {
        printf("Error: Failed to pack packet with code %d\n", packed_size);
        return 1;
    }

    printf("Successfully packed %d bytes.\n", packed_size);
    printf("Transmitting data: ");

    for (int i = 0; i < packed_size; i++) {
        printf("%02X ", tx_buffer[i]);
    }

    printf("\n\n");

    OSUSatPacket received_packet;
    OSUSatPacketResult result =
        osusat_packet_unpack(&received_packet, tx_buffer, packed_size);

    if (result == OSUSAT_PACKET_OK) {
        printf("Successfully unpacked packet.\n");
        print_packet(&received_packet);

        assert(received_packet.command_id ==
               OSUSatPAYLOADCommand_TRANSFER_IMAGE);
    } else {
        printf("Error: Failed to unpack packet with code %d\n", result);
        return 1;
    }

    return 0;
}
