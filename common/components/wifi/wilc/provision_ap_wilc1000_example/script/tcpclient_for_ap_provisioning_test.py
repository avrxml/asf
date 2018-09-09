from socket import *
serverName = '192.168.5.1'
serverPort = 80
clientSocket = socket(AF_INET, SOCK_STREAM)
clientSocket.connect((serverName,serverPort))
import time
time.sleep(1)
#sentence = "apply,dlink,2,66117000" #WPA (SSID, Security (1 = Open, 2 = WPA, 3 = WEP), Key)
sentence = "apply,dlink,3,1234567890,2,1" #Extra paramters needed for WEP- 4th: Key Index, 5th:Key type (1=open,2=shared,3=any)
clientSocket.send(sentence)
modifiedSentence = clientSocket.recv(1024)
print 'From Server:', modifiedSentence
clientSocket.close()