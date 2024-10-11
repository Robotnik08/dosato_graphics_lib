# compile main.c into a dll
# link main.h with main.c
# Link with the shared library dosato_lib.dll in the dosato_source directory

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Werror -Wno-format

# Include directories
INCLUDES = -I./dosato_source -I./src/include -I./src/include/SDL2

# Linker flags with SDL2, SDL2_image
LDFLAGS_WINDOWS = -L./dosato_source -ldosato_lib -L./src/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
LDFLAGS_LINUX = -L./dosato_source -ldosato_lib -L./src/lib -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm -Wno-format -fPIC

# Source files
SRCS = main.c

# Object files
OBJS = $(SRCS:.c=.o)

# Target
TARGET = graphics.dll

# Detect OS
ifeq ($(OS),Windows_NT)
	LDFLAGS = $(LDFLAGS_WINDOWS)
	RM = del
else
	LDFLAGS = $(LDFLAGS_LINUX)
	RM = rm -f
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -shared -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(TARGET) *.o
