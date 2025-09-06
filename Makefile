# Makefile for building server and client applications

CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC_DIR = src
OBJ_DIR = build
BIN_DIR = build/bin
# Find all .c files recursively
SRCS := $(shell find $(SRC_DIR) -name "*.c")
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Detect platform
ifeq ($(OS),Windows_NT)
PLATFORM_LIBS = -lws2_32
CFLAGS += -D_WIN32 -mconsole
TARGET_SERVER = $(BIN_DIR)/server.exe
TARGET_CLIENT = $(BIN_DIR)/client.exe
else
PLATFORM_LIBS =
TARGET_SERVER = $(BIN_DIR)/server
TARGET_CLIENT = $(BIN_DIR)/client
endif

DEPFLAGS = -MMD -MP
CFLAGS += $(DEPFLAGS)

.PHONY: all clean

all: $(TARGET_SERVER) $(TARGET_CLIENT)

# Server build: uses server, features, protocol, utils
$(TARGET_SERVER): $(OBJS)
	@echo "Linking $@..."
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ \
		$(filter build/server/%.o build/features/%.o build/protocol/%.o build/utils/%.o,$(OBJS)) \
		$(PLATFORM_LIBS)


# Client build: uses client, protocol, utils
$(TARGET_CLIENT): $(OBJS)    
	@echo "Linking $@..."
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ \
		$(filter build/client/%.o build/features/%.o build/protocol/%.o build/utils/%.o,$(OBJS)) \
		$(PLATFORM_LIBS)

# Compile each .c file to .o in build/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Include dependency files
-include $(OBJS:.o=.d)

clean:
	rm -rf $(OBJ_DIR) $(TARGET_SERVER) $(TARGET_CLIENT)
