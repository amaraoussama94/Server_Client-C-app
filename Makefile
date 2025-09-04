# Makefile for building server and client applications

CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC_DIR = src
OBJ_DIR = build

# Find all .c files recursively
SRCS := $(shell find $(SRC_DIR) -name "*.c")
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

TARGET_SERVER = server
TARGET_CLIENT = client

DEPFLAGS = -MMD -MP
CFLAGS += $(DEPFLAGS)

.PHONY: all clean

all: $(TARGET_SERVER) $(TARGET_CLIENT)

# Link server: only use server, features, protocol, utils
$(TARGET_SERVER): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(filter %server/%.o %features/%.o %protocol/%.o %utils/%.o,$(OBJS))

# Link client: only use client, protocol, utils
$(TARGET_CLIENT): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(filter %client/%.o %protocol/%.o %utils/%.o,$(OBJS))

# Compile each .c file to .o in build/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Include dependency files
-include $(OBJS:.o=.d)

clean:
	rm -rf $(OBJ_DIR) $(TARGET_SERVER) $(TARGET_CLIENT)
