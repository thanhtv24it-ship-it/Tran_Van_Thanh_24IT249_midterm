# Makefile for simplified ls(1) implementation
# NetBSD-style options as specified in the project man page

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -pedantic -Iinclude -g
LDFLAGS =

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

SRCS    = $(wildcard $(SRC_DIR)/*.c)
OBJS    = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
TARGET  = ls

.PHONY: all clean dirs

all: dirs $(TARGET)

dirs:
	@mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Convenience target to run a quick self-test
test: $(TARGET)
	@echo "=== Basic listing ==="
	./$(TARGET)
	@echo
	@echo "=== Long format ==="
	./$(TARGET) -l
	@echo
	@echo "=== All files including hidden ==="
	./$(TARGET) -la
	@echo
	@echo "=== With inode and blocks ==="
	./$(TARGET) -lis
	@echo
	@echo "=== Human readable ==="
	./$(TARGET) -lh
	@echo
	@echo "Done."
