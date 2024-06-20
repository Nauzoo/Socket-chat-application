
CC = gcc
FLAGS = -g -Wall

all: server client

server: commons
	$(CC) $(FLAGS) netwCommons.o server.c -o server

client: commons
	$(CC) $(FLAGS) netwCommons.o client.c -o client

commons:
	$(CC) $(FLAGS) -c netwCommons.c -o netwCommons.o
