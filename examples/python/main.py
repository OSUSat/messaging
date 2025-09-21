from osusat_messaging import Destination, MessageType, EPSCommand, OSUSatPacket
from osusat_messaging.messages import PAYLOADCommand

command_payload = b'\x00'

packet_to_send = OSUSatPacket(
    version=1,
    destination=Destination.PAYLOAD,
    source=Destination.OBC,
    message_type=MessageType.COMMAND,
    command_id=PAYLOADCommand.TRANSFER_IMAGE,
    payload=command_payload
)

print(f"Created Packet: {packet_to_send}")
encoded_bytes = packet_to_send.pack()
print(f"Encoded Bytes: {encoded_bytes.hex(' ')}")

try:
    decoded_packet = OSUSatPacket.unpack(encoded_bytes)
    print(f"Decoded Packet: {decoded_packet}")
    print(f"Payload: {decoded_packet.payload.hex(' ')}")
except ValueError as e:
    print(f"Error: {e}")
