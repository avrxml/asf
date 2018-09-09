import socket
import sys

# UDP Server details
UDP_IP = ''
UDP_PORT = 6666

# create and bind socket to specified IP and port
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
try:
    sock.bind((UDP_IP, UDP_PORT))
    sock.settimeout(300)
except (socket.error, socket.timeout) as msg:
    print 'Error - ',msg
    sock.close()
    sock = None
    sys.exit(1)

# receive data from WINC3400 UDP client 
try:
    data, addr = sock.recvfrom(1024)
    print "received message:", data

except (socket.error, socket.timeout) as msg:
    print 'Error - ',msg
finally:
    print 'Closing Socket'
    sock.close()
