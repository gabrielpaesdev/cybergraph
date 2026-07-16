CXX      := g++
CC       := gcc
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
CFLAGS   := -Wall -Wextra -O2
LDFLAGS  := -lSDL2 -lm

SRC_DIR  := .
TARGET   := cybergraph

# Busca tanto os arquivos .cpp quanto os .c
CPP_SRCS := $(wildcard $(SRC_DIR)/*.cpp)
C_SRCS   := $(wildcard $(SRC_DIR)/*.c)

# Gera a lista de objetos (.o) esperados no diretório atual
CPP_OBJS := $(CPP_SRCS:.cpp=.o)
C_OBJS   := $(C_SRCS:.c=.o)

# Junta todos os objetos para o linker
ALL_OBJS := $(CPP_OBJS) $(C_OBJS)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(ALL_OBJS)
	@echo "Linking target: $@"
	$(CXX) $(ALL_OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build successful!"

# Regra para compilar os arquivos C++
%.o: %.cpp
	@echo "Compiling C++: $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para compilar os arquivos C puros (o sfx.c)
%.o: %.c
	@echo "Compiling C: $<"
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	@echo "Running $(TARGET)..."
	./$(TARGET)

clean:
	@echo "Cleaning up object files and executable..."
	rm -f *.o $(TARGET)
	@echo "Clean done!"
