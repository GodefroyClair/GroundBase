//
//  main.c
//  ExtraToolBox
//
//  Created by Manuel Deneu on 09/03/2018.
//  Copyright © 2018 Manuel Deneu. All rights reserved.
//


#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <GBRunLoop.h>
#include <GBTimer.h>

#include <ToolKit/AsyncFDReader.h>

static int fildes[2] = {0}; // 0 is read, 1 is write.


static void onData(GBRunLoop* runLoop , void* data)
{
    
    AsyncFDReader*reader = data;
    assert(data);
    assert( runLoop == reader->mainRunLoop);
    
    
    
    static char buf[128] = {0};
    const size_t sizeToRead = AsyncFDReaderGetReadSize(reader);
    const size_t readBytes = AsyncFDReaderPop(reader, buf, sizeToRead);
    
    
    if( readBytes == GBSizeInvalid)
    {
        printf("Nothing to write\n");
    }
    else
    {
        static int count = 0;
        printf("Did read %zi bytes count %i \n" , readBytes , count++);
        
        if( count >200)
        {
            printf("Send stop\n");
            GBRunLoopStop( AsyncFDReaderGetMainLoop(reader) );
            AsyncFDReaderStop( reader);
        }
    }
     
    
}

// simulate external source (ie incomming serial port) writes.
static void write_entryPoint( GBThread* thread )
{
    uint8_t buffer[64] = {0};
    uint8_t v = 0;
    
    while (GBThreadShouldReturn( thread) == 0)
    {
        usleep(1000*20);
        

        memset(buffer, v, 64);
        
        const ssize_t ret = write(fildes[1], buffer, 64);
        printf("Did Send %zi bytes \n" , ret);
        v++;
        
    }
    
    printf("Writing thread returned \n");
    
}

static void AsyncFDReaderTests()
{
    {
        AsyncFDReader reader;
        
        assert( AsyncFDReaderInit(NULL, NULL) == 0);
        assert( AsyncFDReaderInit(NULL, "somename") == 0);
        assert( AsyncFDReaderRelease(NULL) == 0);
        
        
        assert( AsyncFDReaderInit(&reader, "testQueue") );
        
        assert( AsyncFDReaderGetMainLoop(&reader) == NULL);
        
        assert( AsyncFDReaderGetReadSize(&reader) == AsyncFDReaderDefaultSizeToRead);
        assert( AsyncFDReaderGetReadNotificationAtSize(&reader) == AsyncFDReaderDefaultSizeToNotify);
        
        AsyncFDReaderSetReadSize(&reader, 10);
        assert( AsyncFDReaderGetReadSize(&reader) == 10);
        assert( AsyncFDReaderGetReadNotificationAtSize(&reader) == AsyncFDReaderDefaultSizeToNotify);
        
        AsyncFDReaderSetReadNotificationAtSize(&reader, 20);
        assert( AsyncFDReaderGetReadNotificationAtSize(&reader) == 20);
        assert( AsyncFDReaderGetReadSize(&reader) == 10);
        
        assert( AsyncFDReaderRelease(&reader) );
    }
    {
        AsyncFDReader reader;
        
        
        assert( AsyncFDReaderRelease(NULL) == 0);
        
        
        assert( AsyncFDReaderInit(&reader, "testQueue") );
        
        assert( AsyncFDReaderStart(&reader) == 0);
        
        assert( AsyncFDReaderSetFileDescriptor(NULL, 1, 0) == 0 );
        assert( AsyncFDReaderSetFileDescriptor(&reader, -1, 0) == 0 );
        assert( AsyncFDReaderSetFileDescriptor(&reader, 0, 0) == 0 );
        
        assert( AsyncFDReaderSetFileDescriptor(&reader, 10, 0)  );
        assert( AsyncFDReaderRelease(&reader) );
    }
    
}
int main(int argc, const char * argv[])
{
    AsyncFDReaderTests();
    
    {
        GBThread* writeThread = GBThreadInit();
        GBThreadSetMain(writeThread, write_entryPoint);
        
        
        
        if( pipe( fildes ) != 0)
        {
            return 1;
        }
        
        AsyncFDReader reader;
        
        if( AsyncFDReaderInit( &reader , "myAsyncQueue") == 0)
        {
            printf("AsyncFDReaderInit Error \n");
            
            return 2;
        }
        
        if( AsyncFDReaderSetFileDescriptor(&reader, fildes[0] , 1/* close at release*/ ) == 0)
        {
            return 3;
        }
        

        printf("AsyncFDReaderStart started \n");
        
        GBRunLoop* mainLoop = GBRunLoopInit();
        
        /*
        sendTimer = GBTimerInit(onTime);
        GBTimerSetIntervalMS(sendTimer, 50);
        GBRunLoopAddSource(mainLoop, sendTimer);
        */
        
        AsyncFDReaderSetMainRunLoop(&reader, mainLoop);
        AsyncFDReaderSetReadCallback(&reader, onData);
        
        AsyncFDReaderSetReadSize(&reader, 1);
        AsyncFDReaderSetReadNotificationAtSize(&reader, 128);

        GBThreadStart(writeThread);
        
        if( AsyncFDReaderStart(&reader) == 0)
        {
            return 4;
        }
        
        GBRunLoopRun( mainLoop );
        GBThreadSendAsyncStop( writeThread);
        GBThreadWaitForTerminaison(writeThread);
        
        GBRelease(writeThread);
        GBRelease(mainLoop);
        AsyncFDReaderRelease( &reader);
    }
    
    GBObjectIntrospection(1);
    sleep(5);
    
    return 0;
}
