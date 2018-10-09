//
//  AsyncFDReader.c
//  ExtraToolBox
//
//  Created by Manuel Deneu on 09/03/2018.
//  Copyright © 2018 Manuel Deneu. All rights reserved.
//


#include <stdio.h>
#include <unistd.h> // read
#include "AsyncFDReader.h"
#include <GBFDSource.h>
#include "pipe.h"


const GBSize AsyncFDReaderDefaultSizeToRead = 16;
const GBSize AsyncFDReaderDefaultSizeToNotify = 64;

static void Internal_ThreadMain( GBThread *thread);

BOOLEAN_RETURN uint8_t AsyncFDReaderInit( AsyncFDReader* reader , const char* queueName)
{
    if (reader)
    {
        reader->userContext = NULL;
        reader->sizeToRead = AsyncFDReaderDefaultSizeToRead;
        reader->readCallback = NULL;
        
        //reader->source = NULL;
        reader->fd  = -1;
        reader->shouldCloseFD = 0;
        reader->thread = GBThreadInit();
        
        if (queueName)
        {
            GBThreadSetName(reader->thread, GBSTR(queueName));
        }
        
        GBThreadSetMain(reader->thread, Internal_ThreadMain );
        GBThreadSetUserContext( reader->thread, reader);
        
        reader->pipe = pipe_new(1 , 0);
        
        reader->consumer = pipe_consumer_new( reader->pipe);
        reader->producer = pipe_producer_new( reader->pipe);
        
        reader->notifyAtSize = AsyncFDReaderDefaultSizeToNotify;
        return reader->pipe != NULL;
    }
    return 0;
}

void AsyncFDReaderSetMainRunLoop( AsyncFDReader* reader , GBRunLoop* mainRunLoop)
{
    if( reader && reader->mainRunLoop != mainRunLoop)
    {
        reader->mainRunLoop = mainRunLoop;
    }
}


void AsyncFDReaderSetReadNotificationAtSize( AsyncFDReader* reader , GBSize size)
{
    reader->notifyAtSize = size;
}

GBSize AsyncFDReaderGetReadNotificationAtSize( const AsyncFDReader* reader)
{
    return reader->notifyAtSize;
}

void AsyncFDReaderSetReadSize( AsyncFDReader* reader , GBSize size)
{
    reader->sizeToRead = size;
}

GBSize AsyncFDReaderGetReadSize( const AsyncFDReader* reader)
{
    return reader->sizeToRead;
}

void AsyncFDReaderSetReadCallback( AsyncFDReader* reader, GBRunLoopAsyncCallback readCallback )
{
    if( reader)
    {
        reader->readCallback = readCallback;
    }
}

BOOLEAN_RETURN uint8_t AsyncFDReaderRelease( AsyncFDReader* reader)
{
    if (reader)
    {
        GBRelease( reader->thread);
        
        
        
        
        
        pipe_free( reader->pipe );
        
        pipe_producer_free( reader->producer );
        pipe_consumer_free( reader->consumer );
        
        if( reader->shouldCloseFD)
        {
            return close( reader->fd) == 0;
        }
        return 1;
    }
    return 0;
}

GBRunLoop* AsyncFDReaderGetMainLoop( const AsyncFDReader*reader)
{
    return reader->mainRunLoop;
}

/*
static void onFDSource( GBRunLoopSource* source , GBRunLoopSourceNotification notification)
{
    AsyncFDReader* reader = GBRunLoopSourceGetUserContext(source);
    if( notification == GBRunLoopSourceCanRead)
    {
        static uint8_t buffer[256];
                
        const GBSize readSize = GBFDSourceRead(source, buffer, reader->sizeToRead);
        
        if( readSize != 0 && readSize != GBSizeInvalid)
        {
            
            pipe_push( reader->producer, buffer, readSize);
            
            //GBRunLoopDispatchAsync(reader->internalRunLoop, dispatch_onMainLoop, reader);
            
            const size_t pushed = pipe_available_bytes( reader->pipe );
            printf("Pushed : available %zi \n",pushed);
            if( pushed > reader->notifyAtSize )
            {
                GBRunLoopDispatchAsync(reader->mainRunLoop, reader->readCallback, reader);
            }

        }
        else
        {
            
        }
    }
}
*/

BOOLEAN_RETURN uint8_t AsyncFDReaderSetFileDescriptor( AsyncFDReader* reader, int fd  , uint8_t autoCloseAtRelease)
{
    if( reader && fd > 0)
    {
        /*
        if (reader->fd > 0)
        {
            
            return 0;
        }
        */
        reader->fd = fd;

        if (autoCloseAtRelease != 0)
        {
            reader->shouldCloseFD = 1;
        }
        
        
        return 1;
    }
    
    return 0;
}


BOOLEAN_RETURN uint8_t AsyncFDReaderStart( AsyncFDReader* reader)
{
    if( reader && reader->fd > 0)
    {
        return GBThreadStart( reader->thread);
    }
    
    return 0;
}

BOOLEAN_RETURN uint8_t AsyncFDReaderStop( AsyncFDReader* reader)
{
    if( reader)
    {
        GBThreadSendAsyncStop(reader->thread);
        return 1;
    }
    
    return 0;
}

static void Internal_ThreadMain( GBThread *thread)
{
    AsyncFDReader* reader = GBThreadGetUserContext(thread);
    
    if( !reader)
        return;
    
    
    if( reader->mainRunLoop == NULL || reader->readCallback == NULL)
    {
        printf("[AsyncFDReader] Warning: No runloop or read callback set\n");
    }
    
    //uint8_t run = 1;
    
    size_t accumSize = 0;
    char buffer[128] = {0};
    
    while (1)
    {
        if( GBThreadShouldReturn(reader->thread) )
        {
            //run = 0;
            break;
        }
        ssize_t ret =  read( reader->fd, buffer, reader->sizeToRead);
        
        if( ret <= 0)
        {
            if( ret == -1)
            {
                perror("read");
            }
            continue;
        }
        accumSize += ret;
        
        
        pipe_push( reader->producer, buffer, ret);
        if( accumSize >= reader->notifyAtSize)
        {
            GBRunLoopDispatchAsync(reader->mainRunLoop, reader->readCallback, reader);
            accumSize = 0;
        }
        
        usleep(1000*10);
        
    }
    
    
    printf("AsyncFDReader.Read thread returned \n");
}

size_t AsyncFDReaderPop(AsyncFDReader* reader , void* target, size_t count)
{
    /*
    if (GBObjectTryLock(reader->internalRunLoop) )
    {
     
        const size_t ret = pipe_pop(reader->consumer, target, count);
        GBObjectUnlock(reader->internalRunLoop);
        return ret;
    }
    
    return GBSizeInvalid;
     */
    return pipe_pop_eager(reader->consumer, target, count);
}





