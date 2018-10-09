
CC=gcc

BUILD_CONFIG = -g -DDEBUG

CFLAGS=   $(BUILD_CONFIG)  -Wall -Wextra 
CFLAGS+=  -pedantic  -D_REENTRANT


CFLAGS+=  -I/usr/local/include/GroundBase

LDFLAGS= -lGroundBase 



SOURCES=$(wildcard src/*.c)

OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE= InteractiveTimerC

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 

.c.o:
	@echo $(PLATFORM)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE)

re: clean all
