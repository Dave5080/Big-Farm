CC=gcc
CFLAGS=-g -Wall -O -Wno-unused-variable -std=c99
LDLIBS=-lm -lrt -pthread

all: farm client clear

farm: farm.o xerrori.o simple_queue.o

client: client.o xerrori.o simple_queue.o

clear:
	rm farm.o client.o xerrori.o simple_queue.o
