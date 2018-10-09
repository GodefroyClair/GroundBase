//
//  testGBTimer.c
//  GroundBase
//
//  Created by Manuel Deneu on 07/10/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h> // rand
#include <GBTimer.h>
#include <GBRunLoop.h>
#include <time.h>
#include <sys/time.h>
#include "testGBTimer.h"

// for implementation tests
#include "../src/GBRunLoop/TimersWheel/TimersWheel.h"
//#include "../src/GBRunLoop/GBTimer_Private.h"

/* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */

static BOOLEAN_RETURN uint8_t Internal_clock_gettime( struct timeval *tv)
{
    const clockid_t clockType = CLOCK_MONOTONIC;
    struct timespec t = (struct timespec){ 0 , 0};
    const uint8_t ret = clock_gettime( clockType, &t) == 0;
    
    if( ret )
    {
        TIMESPEC_TO_TIMEVAL(tv, &t);
        
        return 1;
    }
    return 0;
}

static GBTimeMS Internal_clock_getIntervalInMS( const struct timeval *start ,const struct timeval *end )
{
    DEBUG_ASSERT(start);
    DEBUG_ASSERT(end);
    
    if( start && end)
    {
        struct timeval deltaT;
        timersub(end, start, &deltaT);
        const GBTimeMS timeSpentMS =  (GBTimeMS)((deltaT.tv_sec * 1000 )+ (deltaT.tv_usec / 1000));
        return timeSpentMS;
    }
    
    return GBTimeMSInvalid;
}

static GBTimeMS Internal_clock_getEllapsedInMS( const struct timeval *start )
{
    struct timeval end = (struct timeval){ 0 , 0};
    if(Internal_clock_gettime(&end))
    {
        return Internal_clock_getIntervalInMS(start , &end);
    }
    
    return GBTimeMSInvalid;
}

/* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */

static GBTimer* timer1 = NULL;
static GBTimer* timer2 = NULL;

static void onTime( GBRunLoopSource* timer , GBRunLoopSourceNotification reason);
static void onTime2( GBRunLoopSource* timer , GBRunLoopSourceNotification reason);
static void onTime3( GBRunLoopSource* timer , GBRunLoopSourceNotification reason);

static void onTime( GBRunLoopSource* timer , GBRunLoopSourceNotification reason)
{
    if( reason == GBRunLoopSourceTimerFired)
    {
        if( timer == timer1)
        {
            assert(GBTimerIsActive(timer));
            static int accum  = 0;
            printf("%i - ON time %llu  \n" , accum, GBTimerGetIntervalMS(timer) );
            accum++;
            
            if (accum == 2)
            {
                printf("Remove timer1 \n");
                
                assert(GBRunLoopRemoveSource( GBRunLoopSourceGetRunLoop( timer), timer1));

            }
            else if( accum == 5)
            {
                printf("Send Stop\n");
                GBRunLoopStop(GBRunLoopSourceGetRunLoop(timer));
            }
        }
        else if( timer == timer2)
        {
            static int accum  = 0;
            printf("Timer 2 \n");
            accum++;
            
            if( accum == 3)
            {
                printf("Add timer1 again\n");
                assert( GBRunLoopAddSource(GBRunLoopSourceGetRunLoop(timer), timer1) );
            }
        }
    }
    
}

void testGBTimer()
{
    printf("----- test GBTimer ----- \n");
    
    GBRunLoop* runLoop  = GBRunLoopInit();
    
    
    timer1 = GBTimerInit(onTime);
    
    GBTimerSetIntervalMS(timer1,500);
    
    timer2 = GBTimerInit(onTime);
    
    GBTimerSetIntervalMS(timer2,3000);
    /*
    timer3 = GBTimerCreate(onTime);
    tv.tv_sec  = 5;
    tv.tv_usec = 0;
    GBTimerSetInterval(timer3, &tv);
    GBTimerSetPeriodic(timer3, 0);
    */
    assert(GBRunLoopAddSource(runLoop, timer2));
    
    
    assert(GBRunLoopAddSource(runLoop, timer1));
    assert(GBRunLoopAddSource(runLoop, timer1) == 0);
    
    GBRunLoopRun(runLoop);
    
    GBObjectIntrospection(1);
    GBRelease(runLoop);

    //const int ref = GBObjectGetRefCount(timer1);
    assert(GBObjectGetRefCount(timer1) == 1);
    assert(GBObjectGetRefCount(timer2) == 1);
    GBRelease(timer1);  //Timer already released when removed from RunLoop
    GBRelease(timer2);
    
    /*
    GBRelease(timer3);
     */
}

void testGBTimer2()
{
    printf("----- test GBTimer2 ----- \n");
    
    
    GBRunLoop* runLoop  = GBRunLoopInit();
    
    
    
    timer1 = GBTimerInit(onTime2);
    
    GBTimerSetIntervalMS(timer1,2000);
    GBTimerSetPeriodic(timer1, 0);
    GBTimerSetActive(timer1, 1);
    
    timer2 = GBTimerInit(onTime2);
    
    GBTimerSetIntervalMS(timer2,3000);
    GBTimerSetActive(timer2, 0);
    
    GBTimer* timerSecs = GBTimerInit(onTime2);
    
    GBTimerSetIntervalMS(timerSecs,1000);
    
    printf("Timer 1 %p \n" , (void*)timer1);
    printf("Timer 2 %p \n" , (void*)timer2);
    printf("Timer 3 %p \n" , (void*)timerSecs);
    
    GBRunLoopAddSource( runLoop  ,timerSecs);
    GBRunLoopAddSource( runLoop  ,timer1);
    GBRunLoopAddSource( runLoop  ,timer2);
    
    GBRunLoopRun(runLoop);
    GBRelease(runLoop);
    
    
    GBRelease(timer1);
    GBRelease(timer2);
    GBRelease(timerSecs);

}

static void onTime2( GBRunLoopSource* timer , GBRunLoopSourceNotification reason)
{
    if( reason == GBRunLoopSourceTimerFired)
    {
        static int coutSec = 0;
        if( timer == timer1)
        {
            assert( coutSec >=1 || coutSec <= 3); 
            GBTimerSetActive(timer2, 1);
            printf("On time timer1 -> start timer 2 , stop timer 1\n");
            GBTimerSetActive(timer1, 0);
        }
        else if( timer == timer2)
        {
            
            static int count = 0;
            
            const int expected = 1 + 3*(count+1);
            
            printf("On time timer2 %i %i \n" , coutSec , expected);
            
            assert( coutSec >= expected -1 && coutSec <= expected +1 );
            
            
            if( count++ >2)
            {
                printf("Send stop \n");
                GBRunLoopStop( GBRunLoopSourceGetRunLoop(timer));
            }
        }
        else
        {
            coutSec++;
            printf("Seconds %i\n" , coutSec);
        }
    }
}


static void onTime3( GBRunLoopSource* timer , GBRunLoopSourceNotification reason)
{

    assert(reason == GBRunLoopSourceTimerFired);
    
    GBTimeMS ms = GBTimerGetIntervalMS(timer);
    
    printf("On time %llu \n" , ms );
    
    ms += 100;
    
    GBTimerSetIntervalMS(timer, ms);
    
    if( ms > 1500)
    {
        GBRunLoopStop( GBRunLoopSourceGetRunLoop(timer));
    }
}

void testGBTimer3()
{
    printf("----- test GBTimer3 ----- \n");
    
    
    GBRunLoop* runLoop  = GBRunLoopInit();
    
    GBTimer* timer4 = GBTimerInit( onTime3);
    GBTimerSetIntervalMS( timer4 , 100);
    
    GBRunLoopAddSource( runLoop, timer4);
    
    GBRunLoopRun( runLoop );
    GBRelease( timer4 );
    GBRelease(runLoop);
}


void testGBTimerImpl()
{
    printf("----- test GBTimer Implementation ----- \n");
    
    TimersWheel* timeWheel =  TimersWheelInit();
    assert(timeWheel);
    
    Timer* newTimer = TimerInit(NULL, 0);
    assert(newTimer);
    
    Timer* newTimer2 = TimerInit(NULL, 0);
    assert(newTimer2);
    
    Timer* newTimer3 = TimerInit(NULL, 0);
    assert(newTimer3);
    
    assert(TimerWheelAddTimer(timeWheel, newTimer, 333));
    assert(TimerWheelAddTimer(timeWheel, newTimer2, 1000));
    assert(TimerWheelAddTimer(timeWheel, newTimer3, 4000));
    
    uint8_t done = 0;
    while (done == 0)
    {
        TimerTick timeToWait = TimerWheelGetTimeout(timeWheel);
        //printf("Time to wait before next %llu \n" , timeToWait);
        if( timeToWait)
        {
            
            const int rr = (int)  ((float)( (rand() / RAND_MAX) * 60 ));
            const int realTime =  1+ rr;//(int) timeToWait;
            
            struct timeval start = (struct timeval){ 0 , 0};
            Internal_clock_gettime(&start);
            
            usleep( ( unsigned int)(1000*realTime) );
            
            struct timeval end = (struct timeval){ 0 , 0};
            Internal_clock_gettime(&end);
            
            struct timeval deltaT;
            timersub(&end, &start, &deltaT);
            
            const GBTimeMS realTimeSpent = Internal_clock_getEllapsedInMS(&start);// (deltaT.tv_sec * 1000) + (deltaT.tv_usec/1000) ;
            //printf("Time spend %zi \n" , realTimeSpent);
            
            TimerWheelStep(timeWheel , realTimeSpent);
        }
        else
        {
            
            Timer* nextTimer = NULL;
            
            while ( (nextTimer = TimerWheelGetFiredTimers(timeWheel) ) )
            {
                static int cout = 0;
                
                printf("On Time %llu \n" , TimerGetInterval(nextTimer));
                
                if( TimerGetInterval(nextTimer) == TimerGetInterval(newTimer3) )
                {
                    if(cout++ > 3)
                    {
                        done = 1;
                    }
                }
            }
            
            
        }
    }
    
    
    

    
    
    TimerRelease(newTimer);
    TimerRelease(newTimer2);
    TimerRelease(newTimer3);
    TimerWheelRelease(timeWheel);
}



