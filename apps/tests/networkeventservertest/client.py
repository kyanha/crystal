#!/usr/bin/python
from socket import *
HOST = 'localhost'
PORT = 12345
 
sock = socket(AF_INET, SOCK_STREAM)
sock.connect((HOST, PORT))

while True:
  data = sock.recv(1024)
  print data
  if not data: break
sock.close()
