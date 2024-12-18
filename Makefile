# Compiler
CC = gcc

# Source files
SRCS = src/*.c

# Build to build/
# Create the build directory if it doesn't exist
BUILD := build
$(shell mkdir $(BUILD))


# Executable name
EXEC = $(BUILD)/main.exe

# Build the executable
all: $(EXEC)

# Compile the source files
$(EXEC): $(SRCS)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRCS)
