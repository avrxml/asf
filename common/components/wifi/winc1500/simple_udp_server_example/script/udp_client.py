import socket, sys

# Application settings
ip = '192.168.1.125'
port  = 6666
print 'UDP client settings:\nIP:   %s\nPort: %d' % (ip, port)
print 'Please make sure above values are pointing to your WINC device.\n'

# Open UDP client socket.
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Send UDP packets.
for i in range(0,10):
    sock.sendto(str(i), (ip, port))
    print 'Sent', i, 'to WINC1500'

# Close socket.
sock.close()
