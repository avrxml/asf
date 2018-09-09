from socket import *
serverName = '10.0.1.4'
serverPort = 6666
clientSocket = socket(AF_INET, SOCK_STREAM)
clientSocket.connect((serverName,serverPort))
sentence = "dummy tcp client message"
clientSocket.send(sentence)
#modifiedSentence = clientSocket.recv(1024)
#print 'From Server:', modifiedSentence
clientSocket.close()