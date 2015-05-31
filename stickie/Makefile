APPLICATION = stickie

# joystick configuration
STICK ?= /dev/input/js0
STICK_AX ?= 1
STICK_AY ?= 2
# RIOT node
ANTENNA ?= /dev/ttyUSB1

BINDIR = $(CURDIR)/bin

CC = gcc
CFLAGS = -std=gnu99 -Wall -pthread
INCLUDES =

SRC = $(wildcard *.c)
OBJ = $(SRC:%.c=%.o)

.PHONY: all clean run

all: $(APPLICATION)

run: $(APPLICATION)
	./$(APPLICATION) $(STICK) $(STICK_AX) $(STICK_AY) $(ANTENNA)

$(APPLICATION): $(OBJ)
	gcc $(CFLAGS) -o $(APPLICATION) $^

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES)-c $^ -o $@

clean:
	rm -f *.o $(APPLICATION)
