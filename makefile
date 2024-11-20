CC = g++
CFLAGS = -Wall 

SOURCES = $(shell find programs/src -name '*.cpp')
OBJECTS = $(patsubst programs/src/%.cpp, programs/build/%.o, $(SOURCES))

HEADERS = $(shell find programs/include -name '*.hpp')
COMMON_OBJ = $(patsubst programs/include/%.hpp, programs/build/%.o, $(HEADERS))

TARGETS = programs/test.elf

all: $(TARGETS)

programs/test.elf: $(COMMON_OBJ) programs/build/test.o
	$(CC) $(CFLAGS) -o $@ $^

programs/build/%.o: programs/src/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<
