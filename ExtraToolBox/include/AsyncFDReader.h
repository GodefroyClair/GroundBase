//
//  AsyncFDReader.h
//  ExtraToolBox
//
//  Created by Manuel Deneu on 09/03/2018.
//  Copyright © 2018 Manuel Deneu. All rights reserved.
//

#ifndef AsyncFDReader_h
#define AsyncFDReader_h

#include <GroundBase.h>
#include <GBRunLoop.h>
#include <GBThread.h>
#include <GBFDSource.h>



extern const GBSize AsyncFDReaderDefaultSizeToRead; // The default size to read from input source
extern const GBSize AsyncFDReaderDefaultSizeToNotify; // Read thread will post notifications when this size is reached

/*
 The internal guts of AsyncFDReader are declared here so that instances can be allocated on the stack. BUT, you should not access its members directly, and use
 AsyncFDReader* methods declared below.
 */
typedef struct
{
    //GBRunLoop* internalRunLoop; // This is the internal runLoop that effectively reads the source. Don't touch it.
    GBThread* thread;
    //GBFDSource *source;
    
    int fd;
    int shouldCloseFD;
    
    GBRunLoop* mainRunLoop; // This is a ref to YOUR runloop, i-e the one you with to get read notifications from. Set this with AsyncFDReaderSetMainRunLoop.
    GBRunLoopAsyncCallback readCallback; // This is a ref to YOUR method, Set this with AsyncFDReaderSetReadCallback.
    
    GBSize sizeToRead; // default to
    
    void* pipe; // pipe_t
    void* consumer; // pipe_consumer_t
    void* producer; // pipe_producer_t
    
    size_t notifyAtSize;
    
    void* userContext; // do whatever you want with this pointer, the implementation don't care! ( except NULL assignement in AsyncFDReaderInit)
} AsyncFDReader;

// queue name can be NULL, this is just a way to name the background reader thread.
BOOLEAN_RETURN uint8_t AsyncFDReaderInit( AsyncFDReader* reader , const char* queueName);
BOOLEAN_RETURN uint8_t AsyncFDReaderRelease( AsyncFDReader* reader);

// default val is AsyncFDReaderDefaultSizeToNotify
void AsyncFDReaderSetReadNotificationAtSize( AsyncFDReader* reader , GBSize size);
GBSize AsyncFDReaderGetReadNotificationAtSize( const AsyncFDReader* reader);


// default val is AsyncFDReaderDefaultSizeToRead
void AsyncFDReaderSetReadSize( AsyncFDReader* reader , GBSize size);
GBSize AsyncFDReaderGetReadSize( const AsyncFDReader* reader);

void AsyncFDReaderSetMainRunLoop( AsyncFDReader* reader , GBRunLoop* mainRunLoop);
GBRunLoop* AsyncFDReaderGetMainLoop( const AsyncFDReader*reader);
void AsyncFDReaderSetReadCallback( AsyncFDReader* reader, GBRunLoopAsyncCallback readCallback );

BOOLEAN_RETURN uint8_t AsyncFDReaderSetFileDescriptor( AsyncFDReader* reader, int fd , uint8_t autoCloseAtRelease);
BOOLEAN_RETURN uint8_t AsyncFDReaderStart( AsyncFDReader* reader);
BOOLEAN_RETURN uint8_t AsyncFDReaderStop( AsyncFDReader* reader);

size_t AsyncFDReaderPop(AsyncFDReader* reader , void* target, size_t count);


#endif /* AsyncFDReader_h */
