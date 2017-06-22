#Rohan Krishna Ramkhumar
#rxr353
#Makefile for assignment 3: pipes
#Reference used: Makefile from Jan24 recitation

CC = gcc
OUT = output.o
SRC = as3.c
TXT = output.txt

all: 	as2

as2: 
	$(CC) -o $(OUT) $(SRC)
	 ./$(OUT) >$(TXT)
	
clean:
	rm -f $(OUT)
	rm -f $(SOUT)	
