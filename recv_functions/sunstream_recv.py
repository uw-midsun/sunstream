import socket
import struct
import sunstream_structs
import can # CAN messages
import cantools # DBC decoding
import click # Command line interface
from sunstream_util import print_in_hex_only

class sunstream_receiver:
    def __init__(self, mcast_grp: str, mcast_port: int, msg_callback, datagram_size: int, datagram_socket_timeout: int, dbc_file: str):
        self.mcast_grp = mcast_grp
        self.mcast_port = mcast_port
        self.msg_callback = msg_callback
        self.sunstream_decoder = sunstream_decoder(dbc_file)
        # If the message callback is not specified, alert the user we will simply print messages
        if(self.msg_callback == None):
            click.secho("Message callback not specified, printing messages to screen", fg="yellow")
        self.datagram_socket_timeout = datagram_socket_timeout
        self.datagram_size = self.sunstream_decoder.compute_datagram_size()
        # Echo the datagram size
        click.secho("Datagram size: " + str(self.datagram_size), fg="blue")
        self.init_multicast_recv()
        click.secho("Initializing sunstream receiver", fg="green")
        click.secho("Runtime iterations are not being reported - it is normal to see no text\nText will only be shown in an error...", fg="yellow")

    def init_multicast_recv(self) -> None:
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind(('', self.mcast_port))
        mreq = struct.pack("4sl", socket.inet_aton(self.mcast_grp), socket.INADDR_ANY)
        self.sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)   
        self.sock.settimeout(self.datagram_socket_timeout)
        # Echo the multicast group and port we have subscribed to
        click.secho("Subscribed to multicast group: " + self.mcast_grp + " on port: " + str(self.mcast_port), fg="blue")
    
    def recv_step(self):
        try:
            datagram = self.sock.recv(self.datagram_size)
            # Decode the header but only send the first 3 bytes to the function
            header_dict = sunstream_structs.datagram_header_struct.unpack(datagram[:sunstream_structs.datagram_header_struct.size])
            # Iter through all of the can messages, storing a pointer variable that increments by the size of each can message
            can_msg_ptr = sunstream_structs.datagram_header_struct.size
            for i in range(header_dict[1]):
                # Send the raw bytes of the can message to the decoder
                # Compile message to send
                msg_raw = datagram[can_msg_ptr:can_msg_ptr+sunstream_structs.can_message_struct.size]
                #print_in_hex_only(msg_raw)
                decoded_msg = self.sunstream_decoder.decode_can_message(msg_raw)
                can_msg_ptr += sunstream_structs.can_message_struct.size
                if(self.msg_callback != None):
                    # Send the decoded message to the callback function
                    self.msg_callback(decoded_msg)
                else:
                    click.secho(decoded_msg, fg="blue")
                    pass
                if(can_msg_ptr == len(datagram)):
                    break
        except socket.timeout:
            click.secho("Datagram socket timeout... is the transmitter turned on?", fg="red")
            pass

class sunstream_decoder:
    def __init__(self, dbc_filepath: str) -> None:
        if(dbc_filepath is None):
            # Error that no dbc file was provided
            click.secho("No dbc file provided, decoding will not be possible. Returning dict of raw can messages", fg="red")
        self.dbc_filepath = dbc_filepath
    
    def decode_datagram_header(self, datagram_header: bytes) -> dict:
        datagram_header_struct = sunstream_structs.datagram_header_struct
        datagram_header_dict = dict(zip(['magic_number', 'can_message_count'], datagram_header_struct.unpack(datagram_header)))
        return datagram_header_dict

    def decode_can_message(self, can_message: bytes) -> can.Message:
        can_message_struct = sunstream_structs.can_message_struct
        can_message_dict = dict(zip(['magic_number','timestamp', 'can_id', 'can_data_length', 'can_data'], can_message_struct.unpack(can_message)))
        # truncate data to data length
        can_message_dict['can_data'] = can_message_dict['can_data'][:can_message_dict['can_data_length']]
        msg : can.Message = can.Message(arbitration_id=can_message_dict['can_id'], data=can_message_dict['can_data'], timestamp=can_message_dict['timestamp'])
        return msg

    def compute_datagram_size(self) -> int:
        # Compute the datagram size based on adding the header size to the number of can messages times the size of each can message
        datagram_size = sunstream_structs.datagram_header_struct.size + (sunstream_structs.can_messages_per_datagram * sunstream_structs.can_message_struct.size)
        return datagram_size

def dump_test(msg: can.Message):
    pass

if __name__ == '__main__':
    MCAST_GRP = '224.1.1.1'
    MCAST_PORT = 5007
    receiver = sunstream_receiver(MCAST_GRP, MCAST_PORT, dump_test, 13443, 2, None)
    while True:
        receiver.recv_step()
