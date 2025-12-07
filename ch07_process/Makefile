# Define the compiler
CC = gcc

# Define the compiler flags
CFLAGS = -pthread

# Define the target executable
TARGET = tadd

# Define the source files
SRCS = tadd.c

# Define the object files
OBJS = $(SRCS:.c=.o)

# Default rule to build the target
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(OBJS)
    $(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to build the object files
%.o: %.c
    $(CC) $(CFLAGS) -c $< -o $@

# Clean up the build files
clean:
    rm -f $(TARGET) $(OBJS)
