# Nome dos compiladores e flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

# Pastas
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin

# Fontes
SERVER_SRC = $(SRC_DIR)/server.c $(SRC_DIR)/utils.c
CLIENT_SRC = $(SRC_DIR)/client.c $(SRC_DIR)/utils.c

# Binários
SERVER_BIN = $(BIN_DIR)/server
CLIENT_BIN = $(BIN_DIR)/client

.PHONY: all clean

all: $(BIN_DIR) $(SERVER_BIN) $(CLIENT_BIN)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(SERVER_BIN): $(SERVER_SRC) $(INC_DIR)/utils.h
	$(CC) $(CFLAGS) $(SERVER_SRC) -o $(SERVER_BIN)

$(CLIENT_BIN): $(CLIENT_SRC) $(INC_DIR)/utils.h
	$(CC) $(CFLAGS) $(CLIENT_SRC) -o $(CLIENT_BIN)

clean:
	rm -rf $(BIN_DIR)
