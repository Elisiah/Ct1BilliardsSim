# Makefile to compile all C files in the "src" directory into an executable

CC = gcc
CFLAGS = -Wall -O2 -Wno-missing-braces -Wno-strict-aliasing
INCLUDES = -Iinclude
SRCDIR = src
BUILD_DIR = build
TARGET = ct1_billiards_sim
SOURCES = $(wildcard $(SRCDIR)/*.c)

# Object files (generated in the "build" directory)
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

# Default target (build the executable)
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJECTS)
	@$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ -lm

# Rule to compile C source files into object files
$(BUILD_DIR)/%.o: $(SRCDIR)/%.c | $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Create the "build/" directory if it doesn't exist
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Clean up the generated files
clean:
	rm -f $(TARGET)
	rm -rf $(BUILD_DIR)

.PHONY: all clean
