//
//  testAsyncCall.c
//  GroundBase
//
//  Created by Manuel Deneu on 14/11/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#include <unistd.h>
#include <sys/time.h>
#include <GroundBase.h>
#include <GBRunLoop.h>
#include <stdio.h>
#include "testAsyncCall.h"

static void asyncTest1(GBRunLoop *runLoop  , void* data);
static void asyncTest2(GBRunLoop *runLoop  , void* data);

static GBTimeMS timeToWait = 500;


static void asyncTest2(GBRunLoop *runLoop  , void* data)
{
    assert(runLoop);
    static int v = 0;
    const int c = (int) data;
    
    
    printf("Receive Dispatch num %i\n" , c);
    assert( c == v++);
    
    
    
}

static double timeInAsync = 0;

static void asyncTest1( GBRunLoop *runLoop  , void* data)
{
    static int c = 0;
    
    UNUSED_PARAMETER(data);
    
    
    
    struct timeval  tv;
    gettimeofday(&tv, NULL);
    const double time_in_mill = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;
    const double diff = time_in_mill - timeInAsync;
    printf("%i Diff time %f \n", c , diff);
    timeInAsync = time_in_mill;

    assert(timeInAsync >= timeToWait );
    
    if( c++ > 10)
    {
        GBRunLoopStop( runLoop);
    }
    else
    {
        GBRunLoopDispatchAfter(runLoop, asyncTest1, NULL, timeToWait);
        
    }
}


void testAsyncCall()
{
    printf("----- test AsyncCall ----- \n");
    
    
    GBRunLoop* runLoop = GBRunLoopInit();
    
    for(uint8_t i = 0;i<10 ;i++)
    {
        printf("Send Dispatch num %i\n" , i);
        GBRunLoopDispatchAsync(runLoop, asyncTest2, (void*)i);
    }
    
    
    struct timeval  tv;
    gettimeofday(&tv, NULL);
    timeInAsync = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;
    
    GBRunLoopDispatchAfter(runLoop, asyncTest1, NULL , timeToWait);
    
    //GBRunLoopDispatchAfter(runLoop, asyncTest1, NULL, 4000);
    GBRunLoopRun(runLoop);
    
    GBRelease(runLoop);
}

/* **** **** **** **** **** **** **** **** **** **** **** */

static void tooLongCall( GBRunLoop *runLoop  , void* data)
{
    UNUSED_PARAMETER(runLoop);
    UNUSED_PARAMETER(data);
    printf("Long call\n");
    sleep(3);
}

static int maxIters = 5;
static void recursiveCall( GBRunLoop *runLoop  , void* data)
{
    int *aa = data;
    DEBUG_ASSERT(aa);
    
    printf("Iter %i \n" , *aa);
    (*aa)++;
    
    if( (*aa) == maxIters)
    {
        GBRunLoopStop(runLoop);
        
    }
    else
    {
        GBRunLoopDispatchAsync(runLoop, tooLongCall, NULL);
        GBRunLoopDispatchAfter( runLoop, recursiveCall, data, 2000);
    }
}



void testAsyncCall2(void)
{
    GBRunLoop* rl = GBRunLoopInit();
    
    int a = 0;
    GBRunLoopDispatchAsync(rl, recursiveCall, &a);
    GBRunLoopDispatchAsync(rl, tooLongCall, NULL);
    
    GBRunLoopRun(rl);
    assert( a == maxIters);
    GBRelease(rl);
}


