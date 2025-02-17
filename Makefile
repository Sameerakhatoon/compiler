BUILD_DIR = build
SOURCE_DIR = src
TEST_DIR = test
TARGET = $(BUILD_DIR)/main  # Change output executable to "main"
CC = gcc
CFLAGS = -g
LDFLAGS = -g

# Find all .c files in src/ and src/helpers/, excluding src/test.c
SOURCES = $(filter-out $(SOURCE_DIR)/test.c, $(wildcard $(SOURCE_DIR)/*.c) $(wildcard $(SOURCE_DIR)/helpers/*.c))

# Convert .c files to object files in respective build directories
OBJECTS = $(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/src/%.o, $(SOURCES))

# Ensure build directory exists before linking
$(TARGET): $(OBJECTS)
	mkdir -p $(BUILD_DIR)/src $(BUILD_DIR)/src/helpers $(BUILD_DIR)/test
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

# Rule for compiling source files to object files in BUILD_DIR/src
$(BUILD_DIR)/src/%.o: $(SOURCE_DIR)/%.c
	mkdir -p $(dir $@)  # Ensure the directory for object file exists
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule that does not delete .keep file
clean:
	rm -f $(BUILD_DIR)/src/*.o $(BUILD_DIR)/test/*.o $(TARGET)
	find $(BUILD_DIR) -name "*.o" -exec rm -f {} \;

# run the main executable in the build directory
run:
	$(TARGET)

# rub the main executable in debug mode using gdb, also set breakpoints at parse, parse_expression
debug:
	gdb $(TARGET) -ex "break parse" -ex "break parse_expression"
