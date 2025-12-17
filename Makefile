# Detect the Operating System
UNAME_S := $(shell uname -s)

# Compiler and Flags
CC = gcc


# --- AUTOMATIC FILE DISCOVERY ---
# $(wildcard *.c) finds all C files in the current directory
SRC = $(wildcard *.c)
# $(SRC:.c=.o) creates a list of object files based on the source list
OBJ = $(SRC:.c=.o)

# Binary name
EXEC = pacman

# OS-Specific Linker Flags
ifeq ($(UNAME_S), Darwin)
    # Mac OS
    # Note: If you installed via Homebrew on Apple Silicon, you might need to add:
    # -I/opt/homebrew/include -L/opt/homebrew/lib
	CFLAGS = -Wall -Wextra -std=c17 $(shell pkg-config --cflags raylib)
    LDFLAGS = $(shell pkg-config --libs raylib) -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
else
    # Linux
	CFLAGS = -Wall -Wextra -std=c17
    LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

# Build Targets
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

# Generic rule to compile any .c file into a .o file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)
