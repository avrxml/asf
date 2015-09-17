import socket, sys

scIpAddr = '192.168.1.125'
udpPort  = 6666
indexNum = 'Hello!'

# open UDP socket
udpSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udpSock.bind(('', udpPort))

for i in range(0,10):
        # receive message from WINC1500 
        data, addr = udpSock.recvfrom(1024)
        print 'received', data, 'from WINC1500'
	
        # send back packet to WINC1500
        udpSock.sendto(str(i), (scIpAddr, udpPort))
        print 'sent', i, 'to WINC1500'

# close socket
udpSock.close()
