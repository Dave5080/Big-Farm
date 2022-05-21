#!/usr/bin/python3

import socket
import _thread

HOST = "127.0.0.1"
PORT = 8888

def on_new_client(clientsocket,addr):
    sep = "\n"
    
    filename = ''
    sum = ''
    pid = ''
    tid = ''
    while sep not in filename:
        data = clientsocket.recv(2)
        filename += data.decode("utf8")
    while sep not in sum:
        data = clientsocket.recv(2)
        sum += data.decode("utf8")
    while sep not in pid:
        data = clientsocket.recv(2)
        pid += data.decode("utf8")
    while sep not in tid:
        data = clientsocket.recv(2)
        tid += data.decode("utf8")
    print("{} {}/{} >> \t{}\t{}\n".format(addr,pid.replace("\n",""),tid.replace("\n",""),filename.replace("\n",""),sum.replace("\n","")))

s = socket.socket()
s.bind((HOST, PORT))

print("Server started!")
print("Waiting for clients...")

s.listen(5)

while True:
    c, addr = s.accept()
    print('Got connection from {}'.format(addr))
    _thread.start_new_thread(on_new_client,(c,addr))

s.close()