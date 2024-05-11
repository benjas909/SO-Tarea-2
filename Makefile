CC=g++
CCFLAGS=-Wall
SOURCES=main.cpp functions.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=main

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CCFLAGS) $(OBJECTS) -o $(EXECUTABLE)

.cpp.o:
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)