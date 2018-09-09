import socket
import sys

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the port
server_address = ('', 6666)
print 'starting up on', server_address

try:
    sock.bind(server_address)

    # Listen for incoming connections
    sock.listen(1)
    
    sock.settimeout(300)
    # Wait for a connection
    print 'waiting for a connection'

    connection, client_address = sock.accept()
except (socket.error, socket.timeout) as msg:
    print 'Error - ',msg
    sock.close()
    sock = None
    sys.exit(1)

try:
    connection.settimeout(60)
    print 'connection from', client_address

    # Receive the data and retransmit it
    data = connection.recv(1460)
    print 'received', len(data), 'bytes'

    connection.send(data)
    print 'sent data'
except socket.timeout as timeout:
    print 'Error - ',timeout
finally:
    # Clean up the connection
    print 'close tcp'
    connection.close()
    sock.close()

