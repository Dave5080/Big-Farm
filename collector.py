#!/usr/bin/python3

import socket
import _thread

HOST = "127.0.0.1"
PORT = 8888
sep = "\n"

def on_new_worker_client(clientsocket,addr):
    
    while True:
        filename = ''
        sum = ''
        pid = ''
        tid = ''
        while sep not in filename:
            data = clientsocket.recv(2)
            filename += data.decode("utf8")
        if "\r" in filename:
            break
        while sep not in sum:
            data = clientsocket.recv(2)
            sum += data.decode("utf8")
        while sep not in pid:
            data = clientsocket.recv(2)
            pid += data.decode("utf8")
        while sep not in tid:
            data = clientsocket.recv(2)
            tid += data.decode("utf8")
        print("{} {}/{} >> \t{}\t{}".format(addr,pid.replace("\n",""),tid.replace("\n",""),filename.replace("\n",""),sum.replace("\n","")))
    print("Connecion from {} closed.".format(addr))

def on_new_logging_client(clientsocket,addr):
    print("client started")

s = socket.socket()
s.bind((HOST, PORT))

print("Server started!")
print("Waiting for clients...")

s.listen(8)

while True:
    buf = ''
    c, addr = s.accept()
    while sep not in buf:
        data = c.recv(2)
        buf += data.decode("utf8")
     
    if "worker" in buf:
        print('Got worker connection from {}'.format(addr))
        _thread.start_new_thread(on_new_worker_client,(c,addr))
    else: 
        if "client" in buf:
            print('Got client connection from {}'.format(addr))
            _thread.start_new_thread(on_new_logging_client,(c,addr))

s.close()