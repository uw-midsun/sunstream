def print_in_hex_only(packet):
    print(' '.join('{:02x}'.format(x) for x in packet))
