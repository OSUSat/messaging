"""
OSUSat Messaging Package

This package provides the necessary tools to create, serialize, and deserialize
packets according to the OSUSat/SCRT Messaging Standard.
"""

from .packet import OSUSatPacket, calculate_crc

from .messages import *
