CC = g++
CFLAGS = -Wall 

SOURCES = $(shell find programs/src -name '*.cpp')
TESTS = $(shell find programs/tests -name '*.cpp')

OBJECTS = $(patsubst programs/src/%.cpp, programs/build/%.o, $(SOURCES))
OBJECTS += $(patsubst programs/tests/%.cpp, programs/build/%.o, $(TESTS))

HEADERS = $(shell find programs/include -name '*.hpp')
COMMON_OBJ = $(patsubst programs/include/%.hpp, programs/build/%.o, $(HEADERS))

TARGETS = $(patsubst programs/tests/%.cpp, programs/%.elf, $(TESTS))

all: $(TARGETS)

programs/%.elf: programs/build/%.o $(COMMON_OBJ) 
	$(CC) $(CFLAGS) -o $@ $^

programs/build/%.o: programs/tests/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

programs/build/%.o: programs/src/%.cpp programs/include/%.hpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f programs/build/*.o

purge: clean
	rm -f programs/*.elf
