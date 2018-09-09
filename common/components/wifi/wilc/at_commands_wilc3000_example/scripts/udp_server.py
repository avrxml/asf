from socket import *
serverPort = 6666
serverSocket = socket(AF_INET, SOCK_DGRAM)
serverSocket.bind(('', serverPort))
print "The server is ready to receive"
# while 1:
message, clientAddress = serverSocket.recvfrom(64)
print message
print "*********************************************"
serverSocket.sendto('zzzzz', clientAddress)
serverSocket.close()
  #  modifiedMessage = message.upper()
    