//
//  testGBFDSource.c
//  GroundBase
//
//  Created by Manuel Deneu on 25/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include "testGBFDSource.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <GBFDSource.h>
#include <GBRunLoop.h>

#include <unistd.h> // pipe



static void sourceCallback( GBRunLoopSource* source , GBRunLoopSourceNotification notification)
{
    
    if( notification == GBRunLoopSourceCanRead)
    {
        char buf[128];
        const GBSize readBytes =  GBFDSourceRead(source, buf, 128);
        if( readBytes)
        {
            buf[readBytes-1] =0;
            
            printf("Read : '%s'\n" , buf);
            
            if( strcmp(buf, "remove") == 0)
            {
                GBRunLoop* rl = GBRunLoopSourceGetRunLoop(source);
                assert(rl);
                
                GBRunLoopStop(rl);
            }
            
        }
    }
    else
    {
        printf("Received notification %i\n" , notification);
    }
}

void testGBFDSource()
{
    printf("--------Test GBFDSource --------\n");
    
    
    
    GBFDSource* source = GBFDSourceInitWithFD(-1, sourceCallback); 
    assert(source);
    assert(IsKindOfClass(source, GBFDSourceClass));
    assert(GBFDSourceGetFileDescriptor(source) == -1 );
    
    assert(GBRunLoopSourceGetRunLoop(source) == NULL);
    
    
    
    {
        GBRunLoop* rl = GBRunLoopInit();
        
        assert(GBRunLoopAddSource(rl ,source));
        
        assert(GBRunLoopSourceGetRunLoop(source) == rl);
        
        assert(GBRunLoopRemoveSource( rl ,source));
        
        assert(GBRunLoopSourceGetRunLoop(source) == NULL);
        
        GBRelease(rl);
    }
    GBRelease(source);
    
    
    
    
//    interactiveTest();
}

static void onTime( GBRunLoopSource* source , GBRunLoopSourceNotification notification)
{
    static int accum = 0;
    
    assert(notification == GBRunLoopSourceTimerFired);
    
    GBFDSource* producer = GBRunLoopSourceGetUserContext(source);
    assert( producer);
    assert( GBObjectIsValid(producer));
    
    if( accum++ > 10)
    {
        int fd = GBFDSourceGetFileDescriptor(producer);
        assert( fd > 0);
        
        close(fd);
        
        printf("remove timer \n");
        GBRunLoopRemoveSource(GBRunLoopSourceGetRunLoop(source), source);
    }
    else
    {
        printf("ON time\n");
        
        uint8_t bufferIn[128];
        for( uint8_t i= 0; i< 128;i++)
        {
            bufferIn[i] = i;
        }
        
        assert(GBFDSourceWrite(producer, bufferIn, 128 ) == 128);
    }
    
}

static void consummerCallback( GBRunLoopSource* source , GBRunLoopSourceNotification notification)
{
    //static int accum = 0;
    
    if( notification == GBRunLoopSourceCanRead)
    {
        uint8_t bufferIn[128];
        memset(bufferIn, 0, 128);
        
        const GBSize bytesRead = GBFDSourceRead(source, bufferIn, 128);
        assert(bytesRead == 128);
        printf("Did read %zi bytes" , bytesRead);
        
        for( uint8_t i= 0; i< bytesRead;i++)
        {
            assert(bufferIn[i] == i );
        }
        
        
    }
    else if( notification == GBRunLoopSourceDisconnected)
    {
        printf("Producer disconnected \n");
        GBRunLoopStop(GBRunLoopSourceGetRunLoop(source));
        
        GBRunLoopRemoveSource(GBRunLoopSourceGetRunLoop(source), source);
        assert(GBRunLoopSourceGetRunLoop(source) == NULL);
        
    }
    else
    {
        printf("consummer %i \n" , notification);
    }
}

void testGBFDSource2()
{
    int pipes[2];
    pipes[0] = -1;
    pipes[1] = -1;
    
    if(pipe(pipes) != 0)
    {
        return;
    }
    
    GBFDSource* producer = GBFDSourceInitWithFD(pipes[1], NULL);
    assert(producer);
    
    GBFDSource* consummer = GBFDSourceInitWithFD(pipes[0], consummerCallback);
    
    assert(consummer);
    
    GBRunLoop* rl = GBRunLoopInit();
    assert(rl);
    
    
    assert(GBRunLoopContainsSource(rl , consummer) == 0);
    assert(GBRunLoopContainsSource(rl , producer) == 0);
    
    assert(GBRunLoopAddSource(rl, consummer) );
    assert(GBRunLoopAddSource(rl, producer) == 0); //must fail since producer has no callbacks.
    
    assert(GBRunLoopContainsSource(rl , consummer) );
    
    GBRelease(consummer);

    GBTimer* timer = GBTimerInit(onTime);
    GBTimerSetIntervalMS(timer, 200);
    GBRunLoopSourceSetUserContext(timer, producer);
    
    GBRunLoopAddSource(rl, timer);
    GBRelease(timer);
    
    GBRunLoopRun(rl);
    
    GBRelease(rl);
    GBRelease(producer);
    
    close(pipes[0]);
    close(pipes[1]);
    
    
}
