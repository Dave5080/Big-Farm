CC=gcc
CFLAGS=-g -Wall -O -std=c99
LDLIBS=-lm -lrt -pthread

all: farm client clear

farm: farm.o xerrori.o

client: client.o xerrori.o

clear:
	rm farm.o client.o
