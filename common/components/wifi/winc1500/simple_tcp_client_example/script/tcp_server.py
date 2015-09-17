import socket
import sys

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the port
server_address = ('', 6666)
print 'starting up on', server_address
sock.bind(server_address)

# Listen for incoming connections
sock.listen(1)

while True:
    # Wait for a connection
    print 'waiting for a connection'
    connection, client_address = sock.accept()

    try:
        print 'connection from', client_address

        # Receive the data in small chunks and retransmit it
        data = connection.recv(1460)
        print 'received', len(data), 'bytes'
        connection.send(data)
        print 'sent data'

    finally:
        # Clean up the connection
        print 'close tcp'
        connection.close()
