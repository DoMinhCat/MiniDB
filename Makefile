CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2 -g -std=c11 -Iinclude -Ihelper

SRC_DIR = src
HELPER_DIR = helper

# Recursively get all .c files in src/ and helper/
SRC = $(shell dir /S /B $(SRC_DIR)\*.c)
HELPER_SRC = $(shell dir /S /B $(HELPER_DIR)\*.c)

ALL_SRC = $(SRC) $(HELPER_SRC)
OBJ = $(ALL_SRC:.c=.o)

TARGET = MiniDB.exe

all: $(TARGET)

# Link all object files
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Compile rule (works for all .c files)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	del /Q $(OBJ) 2>nul
	if exist $(TARGET) del /Q $(TARGET)

.PHONY: all clean
