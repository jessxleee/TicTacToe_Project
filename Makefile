# Compiler and flags
CC = gcc
CFLAGS =$(shell pkg-config --cflags gtk4) -lSDL2 -lSDL2_mixer
LDFLAGS = $(shell pkg-config --libs gtk4)

# Project files
SRCS = index.c minimax/perfect.c RL-epsilon-greedy/greedy.c
OBJS = $(SRCS:.c=.o)
EXEC = index

# Default target
all: $(EXEC)

# Compile the executable
$(EXEC): $(SRCS)
	$(CC) $(SRCS) -o $(EXEC) $(CFLAGS) $(LDFLAGS)

# Compile object files
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean up build files
clean:
	rm -f $(OBJS) $(EXEC)

# Run the application
run: $(EXEC)
	./$(EXEC)

#Debug the application
debug: CFLAGS += -g
debug: all

release: CFLAGS += -O2
release: all
