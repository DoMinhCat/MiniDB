CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2 -g -std=c11 -Iinclude -Ihelper

SRC_DIR = src
HELPER_DIR = helper

# get all .c in src/ and helper/
SRC = $(shell dir /S /B $(SRC_DIR)\*.c)
HELPER_SRC = $(shell dir /S /B $(HELPER_DIR)\*.c)

ALL_SRC = $(SRC) $(HELPER_SRC)
OBJ = $(ALL_SRC:.c=.o)

TARGET = MiniDB.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# clean
clean:
	del /Q $(OBJ) 2>nul
	if exist $(TARGET) del /Q $(TARGET)

.PHONY: all clean
