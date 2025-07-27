CC = g++
CFLAGS = -std=c++11 -O3 -Wall -I./src
MILITARY_FLAGS = -DCHANGSHA_DEFENSE

SRC_DIR = src
BIN_DIR = bin
TEST_DIR = test

SRCS = $(wildcard $(SRC_DIR)/*/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.o,$(SRCS))
EXEC = $(BIN_DIR)/bcs_simulator

TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp)
TEST_EXEC = $(BIN_DIR)/bcs_test

all: build

build: $(EXEC)

$(EXEC): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(MILITARY_FLAGS) -c $< -o $@

test: $(TEST_EXEC)
	./$(TEST_EXEC)

$(TEST_EXEC): $(filter-out $(BIN_DIR)/Main.o, $(OBJS)) $(TEST_DIR)/test_main.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(BIN_DIR)

run: build
	./$(EXEC)

.PHONY: all build test clean run