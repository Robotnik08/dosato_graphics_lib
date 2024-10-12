# compile main.c into a dll
# link main.h with main.c
# Link with the shared library dosato_lib.dll in the dosato_source directory

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Werror -Wno-format

# Include directories
INCLUDES = -I./dosato_source -I./src/include -I./src/include/SDL2

# Linker flags
LDFLAGS_WINDOWS = -L./dosato_source -llibdosato -L./src/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
LDFLAGS_LINUX = -L./dosato_source -ldosato -L./src/lib -lm -Wno-format -fPIC -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

# Source files
SRCS = main.c

# Object files
OBJS = $(SRCS:.c=.o)

# Target
TARGET = graphics

# Detect OS
ifeq ($(OS),Windows_NT)
	LDFLAGS = $(LDFLAGS_WINDOWS)
	RM = del
	TARGET := $(TARGET).dll
else
	LDFLAGS = $(LDFLAGS_LINUX)
	RM = rm -f
	TARGET := $(TARGET).so
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -shared -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(TARGET) *.o
