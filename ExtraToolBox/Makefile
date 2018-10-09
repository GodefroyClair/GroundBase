CC=cc

BUILD_CONFIG = -g -DDEBUG

INCLUDES = -Iinclude -I/usr/local/include/GroundBase/

X_SOURCE = _DEFAULT_SOURCE

CFLAGS= $(BUILD_CONFIG) -fPIC -Wall $(INCLUDES) -Isrc/ -std=gnu99 -pedantic 
CFLAGS+=-D $(X_SOURCE) -D__STRICT_ANSI__ -D_GNU_SOURCE 


LDFLAGS= -shared -L/usr/local/lib/ -lGroundBase

SOURCES = $(wildcard src/*.c)

OBJECTS=$(SOURCES:.c=.o)

TEST_SOURCES=$(wildcard test/*.c)

EXECUTABLE= libGBToolKit.so
TEST=UnitTests


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE)

install:
	cp $(EXECUTABLE) /usr/local/lib
	mkdir -p /usr/local/include/GroundBase/ToolKit/
	cp include/*.h /usr/local/include/GroundBase/ToolKit/
	ldconfig


tests:
	echo $(TEST_SOURCES)
	$(CC)  $(CFLAGS) $(TEST_SOURCES) -L. -lGroundBase -lGBToolKit -o $(TEST)
