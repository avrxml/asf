import socket

# UDP Server details
UDP_IP = ''
UDP_PORT = 6666

# create and bind socket to specified IP and port
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

# receive data from WINC1500 UDP client 
while True:
	data, addr = sock.recvfrom(1024)
	print "received message:", data