CC = gcc
CFLAGS = -std=c99 -Wall -O2
LIBS = -lraylib -lm -lpthread -ldl -lX11 -lGL -lm

SRC_DIR = src
BUILD_DIR = build
SRC_FILES = $(SRC_DIR)/main.c
OUT_FILE = $(BUILD_DIR)/stringslayer

# Criação do diretório de build
$(shell mkdir -p $(BUILD_DIR))

# Compilação
$(OUT_FILE): $(SRC_FILES)
	$(CC) $(CFLAGS) -o $(OUT_FILE) $(SRC_FILES) $(LIBS)

# Limpeza de arquivos gerados
clean:
	rm -rf $(BUILD_DIR)

# Executa o jogo após compilar
run: $(OUT_FILE)
	./$(OUT_FILE)

.PHONY: clean run
