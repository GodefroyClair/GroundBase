/*
 * Copyright (c) 2017 FlyLab
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
//  GBTimer.c
//  GroundBase
//
//  Created by Manuel Deneu on 25/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include <GBTimer.h>
#include "../GBObject_Private.h"

#include "GBTimer_Private.h"

//#define USE_TIMER_FD

#ifdef USE_TIMER_FD
#include <unistd.h> // close
#include <sys/timerfd.h>
#include "AbstractFileDescriptorSource.h"
#else
#include "TimersWheel/TimersWheel.h"
#include "AbstractRunLoopSource.h"
#endif

static void * GBTimer_ctor(void * _self, va_list * app);
static void * GBTimer_dtor (void * _self);
static uint8_t  GBTimer_equals (const void * _self, const void * _b);
static GBRef GBTimer_description (const void * self);






static const GBObjectClass _GBTimerClass =
{
    sizeof(struct _GBTimer),
    GBTimer_ctor,
    GBTimer_dtor,
    NULL,//RunLoop_clone,
    GBTimer_equals,
    GBTimer_description,
    NULL, // initialize
    NULL, // deinit
    NULL,
    
    NULL,
    (char*)GBTimerClassName
};

GBObjectClassRef GBTimerClass = & _GBTimerClass;


static void * GBTimer_ctor(void * _self, va_list * app)
{
    UNUSED_PARAMETER(app);
    
    GBTimer* self = _self;
    if( self)
    {
        GBTimerCallback callback = va_arg(*app, GBTimerCallback);
        
        if( !callback)
        {
            return NULL;
        }

        if( AbstractRunLoopSourceInit( (AbstractRunLoopSource*) self , callback) == 0)
            return NULL;
        
#ifndef USE_TIMER_FD
        self->_timerImpl = TimerInit( self, 0);
        DEBUG_ASSERT(self->_timerImpl);
#endif
        self->_intervalMS = 0;
        self->_changed = 0;
        self->active = 1;
        
        self->_periodic = 1;
        /*
        self->_timerImpl = NULL;
        
        self->_interval = (struct timeval){ 0 , 0};
        
        
        
        
        
        self->fireCount = 0;
        
        
        */
        return self;
    }
    
    return NULL;
}
static void * GBTimer_dtor (void * _self)
{
    GBTimer* self = _self;
    if( self)
    {
#ifdef USE_TIMER_FD
        if( self->super._fd)
        {
            close( self->super._fd);
        }
#else
        TimerRelease( self->_timerImpl );
#endif
        return self;
    }
    
    return NULL;
}
static uint8_t  GBTimer_equals (const void * _self, const void * _b)
{
    const GBTimer* t1 = (const GBTimer* ) _self;
    const GBTimer* t2 = (const GBTimer* ) _b;

#ifdef USE_TIMER_FD
    return t1->super._fd == t2->super._fd;
#else
    return t1->_timerImpl == t2->_timerImpl;
#endif
}
static GBRef GBTimer_description (const void * _self)
{
    const GBTimer* self = _self;
    if( self)
    {
        return GBStringInitWithCStr("Timer");// GBStringInitWithFormat("Timer %li s %i us"  , self->_interval.tv_sec , self->_interval.tv_usec);
    }
    
    return NULL;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GBTimer* GBTimerInit(GBTimerCallback callback)
{
    
    return GBObjectAlloc( GBDefaultAllocator,GBTimerClass , callback);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GBTimerSetIntervalMS( GBTimer* timer , GBTimeMS ms)
{
    if( timer && ms > 0)
    {
        timer->_intervalMS = ms;
        Internal_GBTimerSetDirty( timer);
    }
}

GBTimeMS GBTimerGetIntervalMS( const GBTimer* timer)
{
    return timer->_intervalMS;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

BOOLEAN_RETURN uint8_t GBTimerIsPeriodic(const GBTimer* timer)
{
    return timer->_periodic;
}
void GBTimerSetPeriodic(GBTimer* timer , uint8_t periodic)
{
    if(timer)
    {
        timer->_periodic = periodic;
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

BOOLEAN_RETURN uint8_t GBTimerIsActive(const GBTimer* timer)
{
    return timer->active;
}

void GBTimerSetActive( GBTimer* timer , uint8_t active)
{
    if( timer )
    {
    
        if( active != timer->active)
        {
            timer->active = active;
            Internal_GBTimerSetDirty(timer);
        }
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void Internal_GBTimerSetDirty( GBTimer* self)
{
    DEBUG_ASSERT( self );
    self->_changed = 1;
}

BOOLEAN_RETURN uint8_t Internal_GBTimerUpdateChanges(GBRunLoop* runLoop, GBTimer* timer)
{
    DEBUG_ASSERT( timer );
    
    if( timer->_changed == 0 )
    {
        return 0;
    }

#ifdef USE_TIMER_FD

    if(  timer->super._fd == 0 )
    {
	timer->super._fd = timerfd_create( CLOCK_MONOTONIC/*CLOCK_REALTIME*/, 0);
    }

    if( timer->super._fd == 0)
	return 0;

    
    
    struct itimerspec newValue;

    newValue.it_value.tv_sec = timer->_intervalMS / 1000;
    newValue.it_value.tv_nsec =  ( timer->_intervalMS % 1000) * 1000000;

    newValue.it_interval.tv_sec =  timer->_intervalMS / 1000;
    newValue.it_interval.tv_nsec = ( timer->_intervalMS % 1000) * 1000000;

    
    if ( timerfd_settime( timer->super._fd ,0/* TFD_TIMER_ABSTIME*/, &newValue, NULL) != 0 )
    {
        
        PERROR("Internal_GBTimerUpdateChanges.timerfd_settime");
        DEBUG_ASSERT(0); // to investigate :)
        return 0;
    }


    timer->_changed = 0;
    return 1;

#else 

    Timer* timerIMP = timer->_timerImpl;
    DEBUG_ASSERT( timerIMP );
    
    if(! TimerWheelRemoveTimer( runLoop->_timersWheel, timerIMP) )
    {
        DEBUG_ASSERT(0);
    }
    
    if( timer->active)
    {
        const GBTimeMS millis = GBTimerGetIntervalMS(timer);
        DEBUG_ASSERT( millis != GBTimeMSInvalid );
        TimerWheelAddTimer(runLoop->_timersWheel, timerIMP, millis);
    }
    timer->_changed = 0;
    
    return 1;
#endif  // USE_TIMER_FD
}
