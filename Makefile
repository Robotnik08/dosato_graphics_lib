# compile main.c into a dll
# link main.h with main.c
# Link with the shared library dosato_lib.dll in the dosato_source directory

# Compiler
# Compiler flags
CFLAGS = -Wall -Werror

# Include directories
INCLUDES = -I./dosato_source -I./src/include

# Linker flags with SDL2
LDFLAGS = -L./dosato_source -ldosato_lib -L./src/lib -lmingw32 -lSDL2main -lSDL2

# Source files
SRCS = main.c

# Object files
OBJS = $(SRCS:.c=.o)

# Target
TARGET = graphics.dll

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -shared -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	if exist $(TARGET) del $(TARGET)
	del *.o
CC = gcc
