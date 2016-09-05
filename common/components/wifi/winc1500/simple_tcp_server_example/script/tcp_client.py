import socket
import sys
import time

# Application settings
ip = '192.168.1.17'
port = 6666
print 'TCP client settings:\nIP:   %s\nPort: %d' % (ip, port)
print 'Please make sure above values are pointing to your WINC device.\n'

# Create a TCP/IP socket.
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening.
server_address = (ip, port)
print 'Connecting to %s port %s...' % server_address
sock.connect(server_address)

try:
    # Send TCP packet to WINC1500 TCP server.
    print 'Sending TCP packet...'
    sock.sendall('Hello')
	
    # WINC1500 TCP server will echo received data back to client.
    data = sock.recv(1460)
    print 'Received', data, 'from WINC1500'

finally:
    print 'closing socket'
    sock.close()
