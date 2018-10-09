
CC=cc
CPP=g++

BUILD_CONFIG = -g -DDEBUG

INCLUDES = -Iinclude
INCLUDES += -I /usr/include/libxml2
#INCLUDES += -I/usr/include/json-c
INCLUDES += -I/usr/local/include/json-c




#X_SOURCE = _BSD_SOURCE
X_SOURCE = _DEFAULT_SOURCE

CFLAGS= $(BUILD_CONFIG) -fPIC -Wall $(INCLUDES) -Isrc/ -std=gnu99 -pedantic 
#CFLAGS+=-D_XOPEN_SOURCE=700 

CFLAGS+=-D $(X_SOURCE) -D__STRICT_ANSI__ -D_GNU_SOURCE 
CFLAGS+=-DUSE_TIMER_FD



LDFLAGS= -lpthread -lxml2 -shared -L/usr/local/lib/ -ljson-c -luuid  -lbsd
#LDFLAGS+=-lcurl 
#-lusb-1.0

#LDFLAGS+=-lwebsockets 


SOURCES = $(wildcard src/*.c) $(wildcard src/*/*.c) $(wildcard src/*/*/*.c)
#SOURCES += src/Private/USB/RawHid/hid_LINUX.c

SOURCES := $(filter-out src/GBRunLoop/GBWebSocket.c, $(SOURCES))

TEST_SOURCES = $(wildcard UnitTestsSources/*.c)
#TEST_SOURCES := $(filter-out TestGroundBase/testGBWebSocket.c, $(TEST_SOURCES))
TEST_SOURCES_CPP = $(wildcard GBCPP/tests/*.cpp)

OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE= libGroundBase.so

TEST = UnitTests
TEST_CPP = UnitTestsCPP
TESTCLIENT = Client
TESTSERVER = Server

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@


install:
	cp $(EXECUTABLE) /usr/local/lib
	mkdir -p /usr/local/include/GroundBase
	cp include/*.h /usr/local/include/GroundBase/
	ldconfig

installCPP:
	cp GBCPP/include/*.hpp /usr/local/include/GroundBase/

uninstall:
	rm -f /usr/local/lib/$(EXECUTABLE)

test:
	$(CC)  $(CFLAGS) $(TEST_SOURCES) -L. -lGroundBase -o $(TEST) -lpthread

testCpp:
	$(CPP) -std=gnu++11 $(INCLUDES) $(TEST_SOURCES_CPP) -IGBCPP/include/ -L. -lGroundBase -o $(TEST_CPP)

testClient :
	$(CC) $(CFLAGS) TestClientServer/Client/main.c -L. -lGroundBase -o $(TESTCLIENT) -lpthread

testServer :                                                                  
	$(CC) $(CFLAGS) TestClientServer/Server/main.c -L. -lGroundBase -o $(TESTSERVER) -lpthread

clean:
	rm -f $(OBJECTS)

fclean: clean
	rm -f $(EXECUTABLE)
	rm -f $(TEST)

purge: fclean uninstall

re: fclean all

