ifeq ($(OS), Windows_NT) 
	INCLUDE = C:\msys64\mingw64\include\SDL2
	LDFLAGS = -lmingw32 -lSDL2main -lSDL2_image -lSDL2_ttf -lSDL2_net -lSDL2 -mwindows -mconsole -lm
	LIBS := -L C:\msys64\mingw64\lib
else 
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S), Linux)
		INCLUDE = /usr/include/SDL2
		LDFLAGS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -lm
	endif
	ifeq ($(UNAME_S), Darwin)
		INCLUDE = /opt/homebrew/include
		LIBS := -L /opt/homebrew/lib
		LDFLAGS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -lm
	endif
endif

CC=gcc
CFLAGS = -g -I$(INCLUDE) -c 

OBJ := server
SRC_DIR := src/
OBJ_DIR := obj/

# src
SOURCE_FILES := $(wildcard $(SRC_DIR)*.c)
OBJECT_FILES += $(patsubst $(SRC_DIR)%.c,$(OBJ_DIR)%.o,$(SOURCE_FILES))

# entity
FOLDER := entity
OBJECT_FILES := $(OBJECT_FILES) $(patsubst $(SRC_DIR)/$(FOLDER)/%.c,$(OBJ_DIR)%.o,$(wildcard $(SRC_DIR)$(FOLDER)/*.c))

# lib
OBJECT_FILES := $(OBJECT_FILES) $(patsubst ../lib/%.c,$(OBJ_DIR)%.o,$(wildcard ./../lib/*.c))

# Build the intermediate files
$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	$(CC) $(CFLAGS) $^ -o $@

# Build the binary by linking the intermediate files
$(OBJ): $(OBJECT_FILES)
	$(CC) $^ -o $(OBJ) $(LDFLAGS) $(LIBS)
	echo $(OBJECT_FILES)

start:
	clear && make && ./$(OBJ)

clean: $(OBJ_DIR)*.o
	rm $^ && rm $(OBJ)

