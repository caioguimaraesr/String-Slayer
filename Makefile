CC = gcc
CFLAGS = -std=c99 -Wall -O2 -Iinclude
LIBS = -lraylib -lm -lpthread -ldl -lX11 -lGL

# Diretórios
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

# Arquivos
SRC_FILES = $(SRC_DIR)/main.c
HEADER_FILES = $(wildcard $(INCLUDE_DIR)/*.h)
OUT_FILE = $(BUILD_DIR)/stringslayer

# Criação do diretório de build
$(shell mkdir -p $(BUILD_DIR))

# Compilação principal
$(OUT_FILE): $(SRC_FILES) $(HEADER_FILES)
	$(CC) $(CFLAGS) -o $(OUT_FILE) $(SRC_FILES) $(LIBS)

# Limpeza
clean:
	rm -rf $(BUILD_DIR)

# Execução
run: $(OUT_FILE)
	./$(OUT_FILE)

.PHONY: clean run