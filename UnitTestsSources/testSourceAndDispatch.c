//
//  testSourceAndDispatch.c
//  UnitTests
//
//  Created by Manuel Deneu on 15/10/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include <GBFDSource.h>
#include <GBRunLoop.h>

#include "testSourceAndDispatch.h"

static const GBTimeMS timeToWait = 100;
static double timeInAsync = 0;

GBFDSource* producer = NULL;

static void after(GBRunLoop* runLoop , void* data)
{
    static int numBeforeStop = 0;
    uint8_t count = (uint8_t) data;
    struct timeval  tv;
    gettimeofday(&tv, NULL);
    const double time_in_mill = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;
    const double diff = time_in_mill - timeInAsync;
    printf("%i Diff time %f \n", count, diff );
    timeInAsync = time_in_mill;
    
    assert(diff >= timeToWait );
    assert(producer);
    
    if( numBeforeStop++ > 10)
    {
        GBRunLoopStop(runLoop);
    }
    
    uint8_t c = 0;
    GBFDSourceWrite(producer, &c, 1);
    
    
}
static void consummerCallback( GBRunLoopSource* source , GBRunLoopSourceNotification notification)
{
    if( notification == GBRunLoopSourceCanRead)
    {
        
        
        uint8_t b = 0;
        if (GBFDSourceRead(source, &b, 1))
        {
            printf("Got data\n");
            
            if( b == 0)
            {
                GBRunLoop* rl = GBRunLoopSourceGetRunLoop(source);
                assert(rl);
            
                struct timeval  tv;
                gettimeofday(&tv, NULL);
                timeInAsync = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;
            
                GBRunLoopDispatchAfter(rl, after, b, timeToWait);
            }
        }
        
    }
}

void testSourceAndDispatch()
{
    printf("--------Test Source + Dispatch --------\n");
    
    
    int pipes[2];
    pipes[0] = -1;
    pipes[1] = -1;
    
    if(pipe(pipes) != 0)
    {
        return;
    }
    
    producer = GBFDSourceInitWithFD(pipes[1], NULL);
    assert(producer);
    
    GBFDSource* consummer = GBFDSourceInitWithFD(pipes[0], consummerCallback);
    
    assert(consummer);
    
    GBRunLoop* rl = GBRunLoopInit();
    assert(rl);
    
    assert(GBRunLoopAddSource(rl, consummer));
    
    uint8_t count = 0;
    GBFDSourceWrite(producer, &count, 1);
    
    for (uint8_t i = 0; i <255; i++)
    {
        count++;
        GBFDSourceWrite(producer, &count, 1);
    }
    
    
    GBRunLoopRun(rl);
    
    
    GBRelease(producer);
    GBRelease(consummer);
    GBRelease(rl);
    
    
}
