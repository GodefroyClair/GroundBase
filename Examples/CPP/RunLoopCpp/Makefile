
CC=g++

BUILD_CONFIG = -g -DDEBUG

CFLAGS=   $(BUILD_CONFIG) -std=c++11  -Wall -Wextra 
CFLAGS+=  -pedantic  -D_REENTRANT


CFLAGS+=  -I/usr/local/include/GroundBase

LDFLAGS= -lGroundBase 



SOURCES=$(wildcard src/*.cpp)

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE= RunLoopCpp

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 

.cpp.o:
	@echo $(PLATFORM)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE)

re: clean all
