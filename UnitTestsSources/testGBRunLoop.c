//
//  testGBRunLoop.c
//  GroundBase
//
//  Created by Manuel Deneu on 25/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include "testGBRunLoop.h"
#include <GBRunLoop.h>

#include <stdio.h>


static void async3(GBRunLoop* runLoop , void* data)
{
    assert(runLoop);
    
    
    int *count = (int *) data;
    assert(count);
    assert(*count == 3);
    
    *count = *count+1;
    
    GBRunLoopStop(runLoop);
}

static void async2(GBRunLoop* runLoop , void* data)
{
    assert(runLoop);
    
    
    int *count = (int *) data;
    assert(count);
    assert(*count == 2);
    
    *count = *count+1;
    
    GBRunLoopDispatchAsync(runLoop, async3, data);
}



static void async0(GBRunLoop* runLoop , void* data)
{
    assert(runLoop);
    
    
    int *count = (int *) data;
    assert(count);
    assert(*count == 0);
    
    *count = *count+1;
}

static void async1(GBRunLoop* runLoop , void* data)
{
    assert(runLoop);
    
    
    int *count = (int *) data;
    assert(count);
    assert(*count == 1);
    
    *count = *count+1;
    
    GBRunLoopDispatchAsync(runLoop, async2, data);
}

void testGBRunLoop()
{
    printf("--------Test GBRunLoop --------\n");
    
    
    GBRunLoop* runLoop  = GBRunLoopInit();
    assert(runLoop);
    
    assert(GBRunLoopContainsSource(NULL , NULL) == 0);
    assert(GBRunLoopContainsSource(runLoop , runLoop) == 0);
    assert(GBRunLoopContainsSource(runLoop , NULL) == 0);
    
    
    assert(GBRunLoopAddSource(NULL, NULL) == 0);
    assert(GBRunLoopAddSource(runLoop, NULL) == 0);
    
    assert(GBRunLoopRemoveSource(NULL, NULL) == 0);
    assert(GBRunLoopRemoveSource(runLoop, NULL) == 0);

    int count = 0;
    GBRunLoopDispatchAsync(runLoop, async0, &count);
    GBRunLoopDispatchAsync(runLoop, async1, &count);
    
    assert(GBRunLoopRun(runLoop));
    
    GBRelease(runLoop);
    
}
