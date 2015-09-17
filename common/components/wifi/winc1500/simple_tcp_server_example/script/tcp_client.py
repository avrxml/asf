import socket
import sys
import time

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ('192.168.1.125', 6666)
print 'connecting to %s port %s' % server_address
sock.connect(server_address)

try:
    # send TCP packet to WINC1500 TCP server
    print 'sending TCP packet'
    sock.sendall('Hello')
	
	# WINC1500 TCP server will echo received data back to client
    data = sock.recv(1460)
    print 'received', data, 'from WINC1500'

finally:
    print 'closing socket'
    sock.close()
