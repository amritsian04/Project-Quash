CC = gcc
CFLAGS = -Wall -g
SRC = src/execute.c src/builtin.c src/parse_interface.c
OBJ = $(SRC:.c=.o)

# Target for the executable
quash: $(OBJ)
	$(CC) -o quash $(OBJ)

# Rule for compiling .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJ) quash

