//
//  main.c
//  InteractiveTimerC
//
//  Created by Manuel Deneu on 17/01/2018.
//  Copyright © 2018 Manuel Deneu. All rights reserved.
//

#include <stdio.h>
#include <unistd.h> // STDIN_FILENO
#include <string.h>
#include <GBRunLoop.h>
#include <GBFDSource.h>

#define BUFFER_MAX_SIZE (size_t) 128

GBTimer* timer1 = NULL;

static void inputCallback( GBRunLoopSource* source , GBRunLoopSourceNotification notification)
{
    if( notification == GBRunLoopSourceCanRead)
    {
        static char buffer [ BUFFER_MAX_SIZE ] = {'0'};
        
        const GBSize readSize = GBFDSourceRead(source, &buffer, BUFFER_MAX_SIZE);
        
        if( readSize)
        {
            buffer[readSize-1] = 0;
            
            if( strcmp(buffer, "quit") == 0)
            {
                GBRunLoop* runloop = GBRunLoopSourceGetRunLoop(source);
                GBRunLoopStop(runloop);
            }
            else if( strcmp(buffer, "pause") == 0)
            {
                printf("%s timer\n" , GBTimerIsActive(timer1)?"Pause":"Resume" );
                GBTimerSetActive(timer1, !GBTimerIsActive(timer1));
            }
        }
    }
}
static void timer1Callback( GBRunLoopSource* source , GBRunLoopSourceNotification notification)
{
    static int count = 0;
    printf("On time %i\n" , count++);
    
    
}

int main()
{
    
    timer1 = GBTimerInit(timer1Callback);
    GBTimerSetIntervalMS(timer1, 1000);
    
    
    GBFDSource* inputSource = GBFDSourceInitWithFD(STDIN_FILENO, inputCallback);
    
    
    GBRunLoop* runloop = GBRunLoopInit();
    
    GBRunLoopAddSource(runloop, timer1);
    GBRunLoopAddSource(runloop, inputSource);
    
    
    printf("Interactive Timer Test : type 'quit' to return, pause to pause/resume the timer \n");
    GBRunLoopRun(runloop);
    
    printf("RunLoop returned \n");
    
    GBRelease(runloop);
    GBRelease(timer1);
    return 0;
}
