from socket import *
from time import *
serverName = '192.168.1.2'
serverPort = 6666
clientSocket = socket(AF_INET, SOCK_DGRAM)

message = ""

for i in range(64):
    message += 'a'
 
while 1:
#    message = raw_input('Input lowercase sentence:')
	clientSocket.sendto(message,(serverName, serverPort))
	print "\r\n" + message
	sleep(1)
#    modifiedMessage, serverAddress = clientSocket.recvfrom(2048)
#    print modifiedMessage
