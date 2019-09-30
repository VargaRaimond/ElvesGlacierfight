# Copyright Varga Raimond 2018

#compilator
CC = gcc 

#flag-uri compilare
CFLAGS = -std=c99 -Wall -Wextra

#executabil
EXE = snowfight

build:functions.h functions.c elf.c 
	$(CC) $(CFLAGS) *.c -o $(EXE) -lm

clean:
	rm -rf $(EXE)