CC=gcc

all: banker client

banker: bank.o banker.c
	$(CC) banker.c bank.o -o banker

client: bank.o client.c
	$(CC) client.c bank.o -o client

bank.o: bank.c bank.h
	$(CC) bank.c -c
