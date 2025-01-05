CC=g++

SRC_DIR = src
INC_DIR = include
BIN_DIR = bin
BIN_NAME = main
TEST_BIN_NAME = tests
LIB_DIR = lib
TESTS_DIR = tests

OBJ_DIR = $(BIN_DIR)/obj
TEST_LIB_DIR = $(LIB_DIR)/munit
TEST_LIB = $(TEST_LIB_DIR)/munit.c

CFLAGS=-I$(INC_DIR)
TFLAGS=-I$(INC_DIR) -I$(TEST_LIB_DIR) -fpermissive -w

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
BIN  = $(BIN_DIR)/$(BIN_NAME)
NON_MAIN_SRC = $(filter-out src/main.c, $(wildcard src/*.c))

$(OBJS): 
	$(CC) -g -c $(patsubst $(OBJ_DIR)%.o, $(SRC_DIR)%.c, $@) -o $@ $(CFLAGS)	

build: $(OBJS)	
	$(CC) -g -o $(BIN) $^ $(CFLAGS)
	$(CC) -g $(TESTS_DIR)/*.c $(NON_MAIN_SRC) $(TEST_LIB) -o $(BIN_DIR)/$(TEST_BIN_NAME) $(TFLAGS)

clean:
	rm -Rf $(OBJ_DIR)/*
	find $(BIN_DIR) -maxdepth 1 -type f -delete

run:
	$(BIN_DIR)/$(BIN_NAME)

tests:
	$(BIN_DIR)/$(TEST_BIN_NAME)

.PHONY: all tests clean run