/*
 * Copyright (c) 2016 FlyLab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
//
//  TimersWheel.c
//  GroundBase
//
//  Created by Manuel Deneu on 21/11/2016.
//  Copyright © 2016 FlyLab. All rights reserved.
//


#include "TimersWheel.h"
#include "timeout.h"
#include <GBAllocator.h>

#define TICK_TIME (timeout_t) 1


const TimerTick TimerTickInvalid = ~TIMEOUT_C(0);

struct _TimersWheel
{
    struct timeouts* timers;
};

TimersWheel* TimersWheelInit(void)
{
    TimersWheel* self = GBMalloc( sizeof( TimersWheel ));
    
    if( self)
    {
        self->timers = timeouts_open( TICK_TIME,  NULL);
        
        return self;
    }
    
    return NULL;
    
}

void TimerWheelRelease( TimersWheel* timerWheel)
{
    timeouts_close( timerWheel->timers);
    GBFree(timerWheel);
}


BOOLEAN_RETURN uint8_t TimerWheelAddTimer( TimersWheel* timerWheel , Timer * timer , TimerTick timePts )
{
    if( timerWheel == NULL)
        return 0;
    
    timeouts_add(timerWheel->timers, timer, timePts );
    
    return 1;
}

BOOLEAN_RETURN uint8_t TimerWheelRemoveTimer( TimersWheel* timerWheel , Timer * timer )
{
    if( timerWheel == NULL)
        return 0;
 
    timeouts_del(timerWheel->timers, timer);
    
    return 1;
}

TimerTick TimerWheelGetTimeout( const TimersWheel* timerWheel)
{
    return timeouts_timeout( timerWheel->timers );
}

BOOLEAN_RETURN uint8_t TimerWheelStep( TimersWheel* timerWheel , TimerTick step)
{
    if( timerWheel == NULL)
        return 0;
    
    timeouts_step( timerWheel->timers, step );
    return 1;
}

Timer* TimerWheelGetFiredTimers( TimersWheel* timerWheel)
{
    return timeouts_get( timerWheel->timers );
}

Timer* TimerInit( void* userData, uint8_t oneShot)
{
    struct timeout * timer = GBMalloc(sizeof(struct timeout));
    
    timer = timeout_init( timer ,  oneShot?TIMEOUT_ABS :  TIMEOUT_INT);
    
    timer->callback.arg = userData;
    
    
    return timer;
}

void TimerRelease(Timer* timer)
{
    if( timer == NULL)
        return;
    
    timeout_del( timer );
    GBFree(timer);
    
}

TimerTick TimerGetInterval( const Timer* timer)
{
    return timer->interval;
}
void* TimerGetUserContext( const Timer* timer)
{
    return timer->callback.arg;
}

