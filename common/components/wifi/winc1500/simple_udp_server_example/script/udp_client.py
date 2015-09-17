import socket, sys

scIpAddr = '192.168.1.125'
udpPort  = 6666
indexNum = 'Hello!'

# open UDP client socket
udpSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# send packet
for i in range(0,10):
    udpSock.sendto(str(i), (scIpAddr, udpPort))
    print 'sent', i, 'to WINC1500'

# close socket
udpSock.close()
