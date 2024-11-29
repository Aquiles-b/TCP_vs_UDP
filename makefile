CC = g++
CFLAGS = -Wall -g

SOURCES = $(shell find project/src -name '*.cpp')
TESTS = $(shell find project/tests -name '*.cpp')

OBJECTS = $(patsubst project/src/%.cpp, project/build/%.o, $(SOURCES))
OBJECTS += $(patsubst project/tests/%.cpp, project/build/%.o, $(TESTS))

HEADDIR=./project/include
HEADERS = $(shell find project/include -name '*.hpp')
COMMON_OBJ = $(patsubst project/include/%.hpp, project/build/%.o, $(HEADERS))

TARGETS = client server

all: $(TARGETS)

client: project/build/client.o $(COMMON_OBJ) 
	$(CC) $(CFLAGS) -o $@ $^

server: project/build/server.o $(COMMON_OBJ) 
	$(CC) $(CFLAGS) -o $@ $^

project/build/%.o: project/src/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

project/build/%.o: project/src/%.cpp project/include/%.hpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f project/build/*.o

purge: clean
	rm -f $(TARGETS)
