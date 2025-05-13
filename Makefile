# Diretórios
SRC_DIR := src
INC_DIR := include
BUILD_DIR := build

# Arquivos-fonte e objeto
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Nome do executável
TARGET := $(BUILD_DIR)/stringslayer-games

# Compilador e flags
CC := gcc
CFLAGS = -I. -Iinclude -Isrc -Wall -Wextra -std=c99
LDFLAGS := -lraylib -lm -ldl -lpthread -lGL  # Ajuste se necessário

# Regra padrão
all: $(TARGET)

# Linkagem
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compilação
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpar arquivos
clean:
	rm -rf $(BUILD_DIR)/*.o $(TARGET)

# Executar o jogo
run: all
	./$(TARGET)
