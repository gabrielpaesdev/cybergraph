
CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
LDFLAGS  := -lSDL2
SRC_DIR   := .
BUILD_DIR := build
TARGET    := $(BUILD_DIR)/cybergraph
SRCS      := $(wildcard $(SRC_DIR)/*.cpp)

OBJS      := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean run

all: $(TARGET)


$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	@echo "Linking target: $@"
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build successful!"


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	@echo "Compiling: $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	@echo "Running $(TARGET)..."
	./$(TARGET)

clean:
	@echo "Cleaning up build directory..."
	rm -rf $(BUILD_DIR)
	@echo "Clean done!"
