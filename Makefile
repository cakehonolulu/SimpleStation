ifdef USE_GCC
# Use GNU's GCC Compiler
CC = gcc
else
# Use LLVM's frontend CLANG
ifdef CODEQL
CC = clang-15
else
CC = clang
endif
endif

# Setup the Windows Compiler (In this cross-compiling using mingw64)
MINGW64 = x86_64-w64-mingw32-gcc-10-win32

# Setup the basic compilation flags
# Warn all, extra and compile for c23
CFLAGS := -Wall -Wextra -Iinclude/
SDLCFLAGS = `sdl2-config --cflags`
SDLLDFLAGS = `sdl2-config --libs`
LDFLAGS := -lm

ifdef LD_MOLD
LDFLAGS += -fuse-ld=mold
endif
ifdef DEBUG_CPU
CFLAGS += -DDEBUG_CPU
endif
ifdef DEBUG_INSTRUCTIONS
CFLAGS += -DDEBUG_INSTRUCTIONS
endif
ifdef DBG
CFLAGS += -g
endif
ifdef ASAN
CFLAGS += -fsanitize=address
LDFLAGS += -fsanitize=address
endif

ifdef PREC23
CFLAGS += -DPREC23
else
CFLAGS += -std=c2x
endif

ifdef TEST
TESTS = Yes
else
TESTS = No
endif

ifeq ($(OS),Windows_NT)
	BINARY := simplestation.exe
	HOST := Windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S), Linux)
		HOST := Linux
		BINARY := simplestation
		ifeq ($(TESTS), Yes)
			C_SOURCES := $(shell find . -name '*.c')
		else
			C_SOURCES := $(shell find . -name '*.c' -not -path "./tests/*")
		endif
		C_OBJECTS = $(C_SOURCES:.c=.o)
		BUILD_DIR = build
		OBJ = $(addprefix $(BUILD_DIR)/,$(addsuffix .0,$(basename $(pathsubst %,%,$(C_SOURCES)))))
		DIR_SORT = $(sort $(dir $(C_SOURCES)))
		DIR_LIST = $(DIR_SORT:./%=%)
		BUILD_OBJ = $(addprefix build/, $(DIR_LIST))
    endif
endif

.PHONY: clean all

all: clean $(BUILD_OBJ) $(BINARY)

$(BINARY): $(C_OBJECTS)
	@echo " 🚧 Linking..."
ifeq ($(HOST), Linux)
	@echo " \033[0;36mLD \033[0msimplestation"
	@$(CC) $(LDFLAGS) $(SDLLDFLAGS) -o $@ $(shell find . -name '*.o')
endif
ifeq ($(HOST), Windows)
	$(MINGW64) $(CFLAGS) -I$(Win32SDL2Headers) -L$(Win32SDL2Libs) $^ -o $@ -lmingw32 -lSDL2main -lSDL2
endif

%.o: %.c
	@$(CC) $(CFLAGS) $(SDLCFLAGS) -c $< -o build/$@
	@echo " \033[0;35mCC\033[0m $<"

$(BUILD_OBJ):
	@echo " 🧩 Preparing build..."

ifeq ($(filter $(BUILD_OBJ), $(MAKECMDGOALS)),)
	$(shell mkdir -p $(BUILD_OBJ);)
endif

clean:
	@echo " 🧹 Cleaning..."

ifneq ($(filter clean, $(MAKECMDGOALS)),)
	$(shell rm -rf $(BUILD_DIR);)
	$(shell rm -rf $(BINARY);)
endif