//
//  main.c
//  TimerC
//
//  Created by Manuel Deneu on 17/01/2018.
//  Copyright © 2018 Manuel Deneu. All rights reserved.
//

#include <stdio.h>
#include <GBRunLoop.h>


/*
 A simple program that run 10 times a 1 second timer.
 For a more complex example see 'InteractiveTimerC' project.
 */

static void timer1Callback( GBRunLoopSource* source , GBRunLoopSourceNotification notification)
{
    static int count = 0;
    printf("On time %i\n" , count);
    
    if( ++count == 10)
    {
        GBRunLoop* runloop = GBRunLoopSourceGetRunLoop(source);
        GBRunLoopStop(runloop);
    }
    
    
}

int main()
{
    
    
    GBTimer* timer1 = GBTimerInit(timer1Callback);
    GBTimerSetIntervalMS(timer1, 1000);
    
    
    GBRunLoop* runloop = GBRunLoopInit();
    
    GBRunLoopAddSource(runloop, timer1);
    
    GBRunLoopRun(runloop);
    
    printf("RunLoop returned \n");
    
    GBRelease(runloop);
    GBRelease(timer1);
    return 0;
}
