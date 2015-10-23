CC=g++
CFLAGS=-g -c
LDFLAGS=-g
SOURCES=autopilot.cpp bigbang.cpp combat.cpp crt.cpp display.cpp main.cpp types.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=../twrl

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
