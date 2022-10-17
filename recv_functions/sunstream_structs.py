import struct

# Header contains the following fields:
# 1. 1 byte: magic number
# 2. 2 bytes: message counts of type can message
# All bytes little endian
datagram_header_struct = struct.Struct('<BH')

# Can message contains the following fields:
# 1 byte: magic number
# 8 bytes: timestamp signed
# 4 byte: can id
# 1 byte: can data length
# 8 bytes: can data
can_message_struct = struct.Struct('<BQIB8s')

can_messages_per_datagram = (10 * 5)