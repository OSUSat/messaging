# OSUSat/SCRT Messaging Standard
- Packet based
- Messages are framed in the same way, with headers, checksums, and ACK/NACK support
- Backwards compatibility is important to make sure we don’t break old commands
- Shared functionality for parsing and serializing between subsystem codebases


| Field           | Size (bytes) | Notes                                                                                                                                                                                                                  |
|-----------------|--------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Start Byte      | 1            | Tentatively `0x7E`. Radio protocols like AX.25 use `0x7E` as a start byte.                                                                                                                                            |
| Version #       | 1            | Messaging standard version number                                                                                                                                                                                      |
| Destination     | 1            | Target destination enum. `0x01` for EPS, `0x02` for payload, etc.                                                                                                                                                      |
| Source          | 1            | Same enum but corresponds to who sent the packet                                                                                                                                                                       |
| Message Type    | 1            | Enum for message type. `0x01` is command, `0x02` telemetry, etc.                                                                                                                                                       |
| Command ID      | 1            | Enum for command ID. With the subsystem destination defined, specific command tables can be different. A common commands table should be maintained with a few commands that every subsystem uses, and a specified offset at which subsystem specific commands begin. |
| Sequence        | 1            | Packet sequence #. Used for sending multi-packet payloads across the wire.                                                                                                                                             |
| Last Chunk Flag | 1            | Indicates whether this packet marks the end of the packet sequence.                                                                                                                                                    |
| Length          | 1            | Packet payload length in bytes                                                                                                                                                                                         |
| Payload         | N            | Variable length payload                                                                                                                                                                                                |
| CRC             | 2            | Cyclic redundancy check for error correction (`CRC-16-CCITT`)                                                                                                                                                          |


# Message Handshake
- Every command must be ACKed with a new packet with the ACK message type
- If an error or invalid state occurs, send back a new packet with the NACK/ERR message type, with the error message/state in the payload

# Command Table Code Generation
- Command table and system configuration lives in a YAML document
- Simple python autogeneration script should parse the YAML configuration and generate C header files and python files containing the command table that can be accessed programmatically
- This way, commands are known at build time

