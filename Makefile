CXX = clang++ # change to clang++ on mac
CC = clang  # change to clang on mac
LD = clang++  # change to clang++ on mac
TARGET_NAME = main

BUILD_DIR := build
BIN_DIR := bin
SRC_DIR := src
LIB_DIR :=

TARGET_EXEC = $(BIN_DIR)/$(TARGET_NAME)

SRCS := $(shell find $(SRC_DIR) -name '*.cc' -or -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

CXXFLAGS := -Wall -Wpedantic -Werror -MMD -MP -O3
CCFLAGS := -Wall -Wpedantic -Werror -MMD -MP -O3
LDFLAGS :=
LIBFLAGS := -lpthread

.PHONY: clean

$(TARGET_EXEC): $(OBJS)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $^ -I/opt/homebrew/opt/argp-standalone/include -L/opt/homebrew/opt/argp-standalone/lib /opt/homebrew/opt/argp-standalone/lib/libargp.a $(LIBFLAGS)

$(BUILD_DIR)/%.cc.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CCFLAGS) -o $@ $<

clean:
	$(RM) -r $(BUILD_DIR) $(BIN_DIR)

-include $(DEPS)
