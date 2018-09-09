from socket import *
from time import *
serverName = '192.168.5.1'#'10.0.1.4'
serverPort = 6666
clientSocket = socket(AF_INET, SOCK_DGRAM)

message = "(^_^)  (-_-) (o_o) (~_~) (*_*)"

#for i in range(32):
#   message += 'a'
 
#while 1:
#    message = raw_input('Input lowercase sentence:')
clientSocket.sendto(message,(serverName, serverPort))
print "\r\nSent : " + message
sleep(1)
#    modifiedMessage, serverAddress = clientSocket.recvfrom(2048)
#    print modifiedMessage
