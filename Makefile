# Makefile for building server and client applications

CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC_DIR = src
OBJ_DIR = build

SRCS = $(wildcard $(SRC_DIR)/**/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

TARGET_SERVER = server
TARGET_CLIENT = client

.PHONY: all clean

all: $(TARGET_SERVER) $(TARGET_CLIENT)

$(TARGET_SERVER): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(filter %server/%.o %features/%.o %protocol/%.o %utils/%.o,$(OBJS))

$(TARGET_CLIENT): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(filter %client/%.o %protocol/%.o %utils/%.o,$(OBJS))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET_SERVER) $(TARGET_CLIENT)
