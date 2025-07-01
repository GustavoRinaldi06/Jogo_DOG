# Compilador
CC = g++

# Caminhos das bibliotecas SDL2
SDL_PATH = C:/SDL2
INCLUDE_PATH = $(SDL_PATH)/include
LIB_PATH = $(SDL_PATH)/lib

# Flags do compilador
CFLAGS = -Wall -g -Iinclude -I$(INCLUDE_PATH)
LDFLAGS = -L$(LIB_PATH) -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

# Diretórios do projeto
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Lista todos os arquivos .cpp dentro de src/ e subdiretórios
SRCS = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/tmxlite/*.cpp) $(wildcard $(SRC_DIR)/tmxlite/detail/*.cpp)

# Cria os objetos baseados nos .cpp, mantendo a estrutura de diretórios
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Nome do executável final
TARGET = $(BIN_DIR)/DOG.exe

# Detecta sistema operacional para ajustar comandos
OS := $(shell uname -s 2>/dev/null || echo Windows)

# Comandos para criar diretórios corretamente no Windows e Linux
ifeq ($(OS), Windows)
    MKDIR = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
    RM = rmdir /S /Q "$(subst /,\,$(1))" 2> NUL || exit 0
else
    MKDIR = mkdir -p $(1)
    RM = rm -rf $(1)
endif

# Compilar tudo
all: folders $(TARGET)

# Criar diretórios, se não existirem
folders:
	$(call MKDIR,$(OBJ_DIR))
	$(call MKDIR,$(OBJ_DIR)/tmxlite)
	$(call MKDIR,$(OBJ_DIR)/tmxlite/detail)
	$(call MKDIR,$(BIN_DIR))

# Compilar o executável
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

# Compilar cada .cpp em .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Limpar arquivos compilados
clean:
	$(call RM, $(OBJ_DIR))
	del /Q $(BIN_DIR)\DOG.exe 2> NUL || exit 0

# Executar o jogo
run: all
	./$(TARGET)

# Modo debug (adiciona símbolos extras)
debug: CFLAGS += -DDEBUG
debug: all
