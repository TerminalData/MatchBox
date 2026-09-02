# Compiler and Flags
CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -O3
CPPFLAGS := -Iinclude -MMD -MP
GTEST_LIBS := -lgtest -lgtest_main -pthread

# Directories
SRC_DIR   := src
TEST_DIR  := tests
OBJ_DIR   := obj
BIN_DIR   := bin

# Targets
TARGET      := $(BIN_DIR)/MatchBox
TEST_TARGET := $(BIN_DIR)/run_tests

# Engine Sources & Objects (excludes main.cpp so tests can link engine code cleanly)
ENGINE_SRCS := $(filter-out $(SRC_DIR)/main.cpp, $(wildcard $(SRC_DIR)/*.cpp))
ENGINE_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/engine/%.o, $(ENGINE_SRCS))

# Main App Objects
MAIN_OBJ := $(OBJ_DIR)/engine/main.o

# Test Sources & Objects
TEST_SRCS := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/tests/%.o, $(TEST_SRCS))

# Header Dependency Files (.d)
DEPS := $(ENGINE_OBJS:.o=.d) $(MAIN_OBJ:.o=.d) $(TEST_OBJS:.o=.d)

# Default: build the engine
.PHONY: all
all: $(TARGET)

# Link Main Executable
$(TARGET): $(MAIN_OBJ) $(ENGINE_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Link Test Runner Executable (Engine logic + Test files, no main.cpp)
$(TEST_TARGET): $(TEST_OBJS) $(ENGINE_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ $(GTEST_LIBS) -o $@

# Compile Application Sources
$(OBJ_DIR)/engine/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)/engine
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# Compile Test Sources
$(OBJ_DIR)/tests/%.o: $(TEST_DIR)/%.cpp | $(OBJ_DIR)/tests
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# Directory Generation
$(BIN_DIR) $(OBJ_DIR)/engine $(OBJ_DIR)/tests:
	mkdir -p $@

# Auto-generate header dependencies
-include $(DEPS)

# Clean Build Artifacts
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run Main Program
.PHONY: run
run: $(TARGET)
	./$(TARGET)

# Build and Run Test Suite
.PHONY: test
test: $(TEST_TARGET)
	./$(TEST_TARGET)
