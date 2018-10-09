//
//  testGBTimer.c
//  GroundBase
//
//  Created by Manuel Deneu on 25/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include "testGBTimerV2.h"
#include <stdio.h>
#include <GBTimer.h>
#include <GBRunLoop.h>

static void timerCallback( GBRunLoopSource* source , GBRunLoopSourceNotification notification)
{
    static int cout = 0;
    assert(notification == GBRunLoopSourceTimerFired);
    assert(GBRunLoopSourceGetRunLoop(source));
    
    
    
    
    if( cout++ == 10)
    {
        GBRunLoopStop( GBRunLoopSourceGetRunLoop(source));
    }
}

void testGBTimerV2()
{
    printf("--------Test GBTimer --------\n");
    
    GBTimer* timer = GBTimerInit(NULL);
    assert(timer == NULL);
    
    timer = GBTimerInit(timerCallback);
    assert(timer);
    assert(IsKindOfClass(timer, GBTimerClass));
    
    assert(GBTimerIsPeriodic(timer));
    
    GBRunLoop* rl = GBRunLoopInit();
    assert(GBRunLoopAddSource(NULL, timer) == 0);
    assert(GBRunLoopSourceGetRunLoop(timer) == NULL);
    assert(GBRunLoopAddSource(rl, NULL) == 0);
    
    GBTimerSetIntervalMS(timer, 100);
    assert(GBTimerGetIntervalMS(timer) == 100);
    
    assert(GBRunLoopAddSource(rl, timer));
    assert(GBRunLoopAddSource(rl, timer) == 0); // 2nd time must fail
    
    assert(GBObjectGetRefCount(timer) == 2 );
    GBRelease(timer);
    assert(GBObjectGetRefCount(timer) == 1 );
    
    assert(GBRunLoopSourceGetRunLoop(timer) == rl);
    
    GBRunLoopRun(rl);
    
    GBRelease(rl);
}
