all: farm client clear

farm: farm.o
	gcc -Wall -o farm farm.o
	chmod +x farm

farm.o: farm.c
	gcc -Wall -c farm.c

client: client.o
	gcc -Wall -o client client.o
	chmod +x client

client.o: client.c
	gcc -Wall -c client.c

clear:
	rm farm.o client.o
