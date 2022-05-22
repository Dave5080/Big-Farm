#!/usr/bin/python3

import socket
import _thread
import threading

coll = dict()
coll_sem = threading.Semaphore()

HOST = "127.0.0.1"
PORT = 8888
sep = "\n"

def fixstr(oldstr):
    return oldstr.replace("\n","").replace("\x00","");

def on_new_worker_client(clientsocket,addr):
    
    while True:
        filename = ''
        sum = ''
        pid = ''
        tid = ''
        while sep not in filename:
            data = clientsocket.recv(1)
            filename += data.decode("utf8")
        if "\r" in filename:
            break
        while sep not in sum:
            data = clientsocket.recv(1)
            sum += data.decode("utf8")
        while sep not in pid:
            data = clientsocket.recv(1)
            pid += data.decode("utf8")
        while sep not in tid:
            data = clientsocket.recv(1)
            tid += data.decode("utf8")
        coll_sem.acquire()
        coll[fixstr(filename)] = int(fixstr(sum))
        print("{} {}/{} >> {} {}".format(addr,fixstr(pid),fixstr(tid),fixstr(filename),coll[fixstr(filename)]))
        coll_sem.release()
    #print("Connecion from {} closed.".format(addr))

def on_new_logging_client(clientsocket,addr):
    mode = ''

    while sep not in mode:
        data = clientsocket.recv(1)
        mode += data.decode("utf-8")

    coll_sem.acquire()
    mydic = sorted(coll.items(), key=lambda x: x[1])

    if "all" in mode:
        #print("mode: all");
        if len(coll) >= 1:
            for (filename, sum) in mydic:
                clientsocket.send("{}\n{}\n".format(filename,sum).encode())
                print("Sending ({}:{} to {})".format(filename, sum, addr))
        else:
            clientsocket.send("Nessun file\n".encode())
            print("Sending Nessun file")
    else:
        mysum = int(fixstr(mode))
        #print("mode: {}", mysum)
        count = 0
        for (filename, sum) in mydic:
            if sum == mysum:
                count += 1
                clientsocket.send("{}\n{}\n".format(filename,sum).encode())
                print("Sending special ({}:{}) to {}".format(filename, sum,addr))
        if count < 1:
            clientsocket.send("Nessun file\n".encode())
            print("Sending Nessun file")
    clientsocket.send("\r".encode())
    coll_sem.release()

s = socket.socket()
s.bind((HOST, PORT))

print("Server started!")
print("Waiting for clients...")

s.listen(8)

while True:
    buf = ''
    c, addr = s.accept()
    while sep not in buf:
        data = c.recv(1)
        buf += data.decode("utf8")
     
    if "worker" in buf:
        #print('Got worker connection from {}'.format(addr))
        _thread.start_new_thread(on_new_worker_client,(c,addr))
    else: 
        if "client" in buf:
            #print('Got client connection from {}'.format(addr))
            _thread.start_new_thread(on_new_logging_client,(c,addr))

s.close()