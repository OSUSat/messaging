import struct
from crccheck.crc import Crc16Ccitt
from . import messages

START_BYTE = 0x7E

HEADER_FORMAT = ">BBBBBBBB" # big-endian
HEADER_SIZE = struct.calcsize(HEADER_FORMAT)

LENGTH_FORMAT = ">B"
LENGTH_SIZE = struct.calcsize(LENGTH_FORMAT)

CRC_FORMAT = ">H" # big-endian
CRC_SIZE = struct.calcsize(CRC_FORMAT)

MIN_PACKET_SIZE = 1 + HEADER_SIZE + LENGTH_SIZE + CRC_SIZE

def calculate_crc(data: bytes) -> int:
    """
        Calculates the CRC-16-CCITT CRC for the given data.
    """
    return Crc16Ccitt.calc(data)

class OSUSatPacket:
    """
        A class to represent, encode, and decode an OSUSat/SCRT messaging packet.
    """
    def __init__(self, version: int, destination: int, source: int,
                 message_type: int, command_id: int, sequence: int = 0,
                 is_last_chunk: bool = True, payload: bytes = b''):

        self.version = version
        self.destination = destination
        self.source = source
        self.message_type = message_type
        self.command_id = command_id
        self.sequence = sequence
        self.is_last_chunk = is_last_chunk
        self.payload = payload

    def __repr__(self):
        """
            Generate a string representation of an OSUSat packet
        """
        dest_name = messages.Destination.__dict__.get(str(self.destination), f'{self.destination:#04x}')

        return (f"OSUSatPacket(dest={dest_name}, "
                f"type={self.message_type:#04x}, cmd={self.command_id:#04x}, "
                f"payload_len={len(self.payload)})")

    def pack(self) -> bytes:
        """
            Serializes the packet object into a bytes object ready for transmission.
        """
        if len(self.payload) > 255:
            raise ValueError("Payload length cannot exceed 255 bytes.")

        header = struct.pack(
            HEADER_FORMAT,
            self.version,
            self.destination,
            self.source,
            self.message_type,
            self.command_id,
            self.sequence,
            1 if self.is_last_chunk else 0,
            len(self.payload),
        )

        # data for CRC includes header and payload
        data_for_crc = header + self.payload
        crc = calculate_crc(data_for_crc)
        packed_crc = struct.pack(CRC_FORMAT, crc)

        return bytes([START_BYTE]) + data_for_crc + packed_crc

    @classmethod
    def unpack(cls, packet_bytes: bytes):
        """
            Deserializes a bytes object into an OSUSatPacket object.
            Raises ValueError if the packet is invalid.

            This method should always be used within an exception handler.
        """
        if not packet_bytes or packet_bytes[0] != START_BYTE:
            raise ValueError(f"Invalid start byte. Expected {START_BYTE:#02x}.")

        if len(packet_bytes) < MIN_PACKET_SIZE:
            raise ValueError(f"Packet too short. Min size is {MIN_PACKET_SIZE} bytes.")

        # verify CRC
        received_crc = struct.unpack(CRC_FORMAT, packet_bytes[-CRC_SIZE:])[0]
        data_to_check = packet_bytes[1:-CRC_SIZE]
        calculated_crc = calculate_crc(data_to_check)

        if received_crc != calculated_crc:
            raise ValueError(f"CRC mismatch. Received {received_crc}, calculated {calculated_crc}.")

        # unpack header fields
        header_fields = struct.unpack(HEADER_FORMAT, data_to_check[:HEADER_SIZE])
        (version, destination, source, message_type, command_id,
         sequence, last_chunk_flag, payload_len) = header_fields

        # extract payload and validate length
        payload = data_to_check[HEADER_SIZE:]
        if len(payload) != payload_len:
            raise ValueError(f"Actual payload length {len(payload)} does not match length in header {payload_len}.")

        return cls(
            version=version, destination=destination, source=source,
            message_type=message_type, command_id=command_id, sequence=sequence,
            is_last_chunk=(last_chunk_flag != 0), payload=payload
        )
