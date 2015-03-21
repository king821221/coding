CC=g++
CFLAGS=-c -g -Wall
all: cart_test
cart_test: cart.o cart_driver.o
	$(CC) cart.o cart_driver.o -g -o cart_test
cart.o:
	$(CC) $(CFLAGS) cart.cpp
cart_driver.o:
	$(CC) $(CFLAGS) cart_driver.cpp
clean:
	rm  -rf *.o  cart_test
