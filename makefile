# Makefile for lab 1

CC = gcc
OBJCS = lab2.c

CFLAGS = -g -Wall -pthread
# setup for system
LIBS =

all: lab2

lab2: $(OBJCS)
	$(CC) $(CFLAGS) -o $@ $(OBJCS) $(LIBS)

clean:
	rm lab2