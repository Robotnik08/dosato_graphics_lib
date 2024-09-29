# compile main.c into a dll
# link main.h with main.c
# Link with the shared library dosato_lib.dll in the dosato_source directory

# Compiler
# Compiler flags
CFLAGS = -Wall -Werror

# Include directories
INCLUDES = -I./dosato_source -I./src/include -I./src/include/SDL2

# Linker flags with SDL2, SDL2_image
LDFLAGS = -L./dosato_source -ldosato_lib -L./src/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

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
