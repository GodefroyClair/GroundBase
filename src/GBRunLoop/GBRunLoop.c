/*
 * Copyright (c) 2016-2018 FlyLab
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
//  GBRunLoop.c
//  GroundBase
//
//  Created by Manuel Deneu on 31/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

//#include <sys/select.h>
#include <string.h>
#include <unistd.h> //pipe
#include <stdlib.h> // free
#include <poll.h>
#include <sys/time.h>
//#include <sys/timerfd.h> // TEMP !!
#include <GBRunLoop.h>
#include <GBString.h>
#include <GBObject_Private.h>
#include <GBAllocator.h>
#include <GBTypes.h>
#include <GBFDSource.h>

#include "../Private/Array.h"
#include "GBRunLoop_Private.h"
#include "AbstractRunLoopSource.h"
#include "AbstractFileDescriptorSource.h"

#include "GBTimer_Private.h"

#ifndef USE_TIMER_FD
#include "TimersWheel/TimersWheel.h"
#endif



static void * RunLoop_ctor(void * _self, va_list * app);
static void * RunLoop_dtor (void * _self);
static uint8_t  RunLoop_equals (const void * _self, const void * _b);
static GBRef RunLoop_description (const void * self);


static GBTimeMS Internal_GBRunLoopPoll(GBRunLoop* self);
static GBTimeMS Internal_GBRunLoopRunOnce(GBRunLoop* self);
static int Internal_GBRunLoopGetTimeToWait(GBRunLoop* self);
static BOOLEAN_RETURN uint8_t Internal_GBRunLoopHandleAsyncCalls(GBRunLoop* self);
static BOOLEAN_RETURN uint8_t Internal_CheckSource( GBRunLoop* runLoop , AbstractFileDescriptorSource* source , struct pollfd *fd);
static void Internal_GBRunLoopUpdateTimers(GBRunLoop *self  , GBTimeMS timeSpent );
BOOLEAN_RETURN uint8_t Internal_clock_gettime( struct timeval *tv);

static const GBObjectClass _RunLoopClass =
{
    sizeof(struct _GBRunLoop),
    RunLoop_ctor,
    RunLoop_dtor,
    NULL,//RunLoop_clone,
    RunLoop_equals,
    RunLoop_description,
    NULL, // initialize
    NULL, // deinit
    NULL,
    NULL,
    (char*)GBRunLoopClassName
};


GBObjectClassRef GBRunLoopClass = & _RunLoopClass;

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

static void * RunLoop_ctor(void * _self, va_list * app)
{
    UNUSED_PARAMETER(app);
    GBRunLoop* self = _self;
    if( self)
    {
        self->_shouldStop = 0;
        self->_fdSources = GBListInit();
        self->_timers  = GBListInit();
        //self->condWait = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
        
        self->_lock =(pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

        self->_runningThread_id = NULL;
        // Async
        self->_asyncCalls = ArrayInit();
        if(pipe(self->_asyncPipeFDs) != 0)
        {
            DEBUG_ASSERT(0);
            return NULL;
        }
        
        self->_userContext = NULL;
        
#ifdef USE_TIMER_FD
#else
        self->_timersWheel = TimersWheelInit();
#endif
        return self;
    }
    return NULL;
}
static void * RunLoop_dtor (void * _self)
{
    GBRunLoop* self = _self;
    if( self)
    {
        if( self->_fdSources)
        {
            GBRelease(self->_fdSources);
        }
        
        if( self->_timers)
        {
            GBRelease(self->_timers);
        }
        
        
        if ( ArrayGetSize(self->_asyncCalls) )
        {
            while (ArrayGetSize(self->_asyncCalls) )
            {
                AsyncCall* p = CONST_CAST(AsyncCall*) ArrayGetValueAtIndex(self->_asyncCalls, 0);
                ArrayRemoveValueAtIndex(self->_asyncCalls, 0);
                GBFree(p);
            }
            
        }
        const void* val = NULL;
        ArrayForEach(self->_asyncCalls, val)
        {
            DEBUG_ASSERT(val);
            
            
        }
        
        ArrayFree(self->_asyncCalls);
            
        if(close(self->_asyncPipeFDs[0]) != 0)
        {
            DEBUG_ASSERT( 0 );
        }
        
        if( close(self->_asyncPipeFDs[1]) != 0)
        {
            DEBUG_ASSERT( 0);
        }
        
        pthread_mutex_destroy( &self->_lock );

#ifndef USE_TIMER_FD    
        TimerWheelRelease( self->_timersWheel );
#endif
        return self;
    }
    return NULL;
}

static uint8_t  RunLoop_equals (const void * _self, const void * _b)
{
    UNUSED_PARAMETER(_self);
    UNUSED_PARAMETER(_b);
    
    const GBRunLoop* a = _self;
    const GBRunLoop* b = _b;
    if (a && b)
    {
        
    }
    return 0;
}
static GBRef RunLoop_description (const void * _self)
{
    const GBRunLoop* self = _self;
    if( self)
    {
        return GBStringInitWithCStr("runloop");
    }
    
    return NULL;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

GBRunLoop* GBRunLoopInit( void )
{
    return GBObjectAlloc( GBDefaultAllocator,GBRunLoopClass );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void  GBRunLoopSetUserContext(GBRunLoop* runLoop , void* context )
{
    if( runLoop)
    {
        runLoop->_userContext = context;
    }
}

void* GBRunLoopGetUserContext( const GBRunLoop* runLoop)
{
    if( runLoop)
    {
        return runLoop->_userContext;
    }
    return NULL;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

GBSize GBRunLoopGetNumTimers( const GBRunLoop* rl)
{
    return GBListGetSize(rl->_timers);
}
GBSize GBRunLoopGetNumFDSources( const GBRunLoop* rl)
{
    return GBListGetSize(rl->_fdSources);
}

GBSize GBRunLoopGetNumSources( const GBRunLoop* rl)
{
    return GBRunLoopGetNumTimers(rl) + GBRunLoopGetNumFDSources(rl);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

static BOOLEAN_RETURN uint8_t GBRunLoopAddFDSource( GBRunLoop* rl , GBFDSource* source)
{
    if( rl && source)
    {
        if ( source->source._callback == NULL)
        {
            return 0;
        }
        
        if( GBListContainsValue(rl->_fdSources, source) && GBRunLoopSourceGetRunLoop(source) != NULL)
            return 0;
        
        if(GBListAddValue( rl->_fdSources , source ))
        {
            AbstractRunLoopSourceSetRunLoop(&source->source, rl);
            return 1;
        }
    }
    return 0;
}

static BOOLEAN_RETURN uint8_t GBRunLoopAddTimer( GBRunLoop* rl , GBTimer* timer)
{
    if( rl && timer)
    {
        if ( ( (AbstractRunLoopSource*)timer)->_callback == NULL)
        {
            return 0;
        }
        if( GBListContainsValue(rl->_timers, timer) && GBRunLoopSourceGetRunLoop(timer) != NULL)
        {
	   return 0;
 	}
        if(GBListAddValue( rl->_timers , timer ))
        {
            AbstractRunLoopSourceSetRunLoop( (AbstractRunLoopSource*)timer, rl);

            timer->_changed = 1;

            const uint8_t  ret = Internal_GBTimerUpdateChanges(rl, timer);

            DEBUG_ASSERT(ret);
            return ret;

        }
    }

    return 0;
}

BOOLEAN_RETURN uint8_t GBRunLoopAddSource( GBRunLoop* rl , GBRunLoopSource* source)
{
    if(rl && source)
    {
        if( IsKindOfClass(source, GBFDSourceClass))
        {
            return GBRunLoopAddFDSource(rl , source);
        }
        else if (IsKindOfClass(source, GBTimerClass))
        {
            return GBRunLoopAddTimer(rl, source);
        }
    }
    return 0;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

static BOOLEAN_RETURN uint8_t GBRunLoopRemoveFDSource( GBRunLoop* rl , GBFDSource* source)
{
    DEBUG_ASSERT(rl);
    DEBUG_ASSERT(source);
    
    if( GBListRemoveValue(rl->_fdSources, source))
    {
        AbstractRunLoopSourceSetRunLoop( &source->source, NULL);
        return 1;
    }
    return 0;

}

static BOOLEAN_RETURN uint8_t GBRunLoopRemoveTimer( GBRunLoop* rl , GBTimer* timer)
{
    DEBUG_ASSERT(rl);
    DEBUG_ASSERT(timer);
    
#ifndef USE_TIMER_FD
    TimerWheelRemoveTimer( rl->_timersWheel, timer->_timerImpl);
#endif
    
    AbstractRunLoopSource* sr = (AbstractRunLoopSource*) timer;
    
    sr->_currentRunLoop = NULL;
    
    if(GBListRemoveValue(rl->_timers, timer))
    {
        return 1;
    }
    
    return 0;
}

BOOLEAN_RETURN uint8_t GBRunLoopRemoveSource( GBRunLoop* rl , GBRunLoopSource* source)
{
    if (rl && source)
    {
        if( IsKindOfClass(source, GBFDSourceClass))
        {
            return GBRunLoopRemoveFDSource(rl , source);
        }
        else if (IsKindOfClass(source, GBTimerClass))
        {
            return GBRunLoopRemoveTimer(rl, source);
        }
    }
    return 0;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

BOOLEAN_RETURN uint8_t GBRunLoopContainsSource(GBRunLoop* runLoop , const GBRunLoopSource*source)
{
    if( runLoop && source)
    {
        if( IsKindOfClass(source, GBFDSourceClass))
        {
            return GBListContainsValue(runLoop->_fdSources, source);
        }
        else if( IsKindOfClass(source, GBTimerClass))
        {
            return GBListContainsValue(runLoop->_timers, source);
        }
        
            
    }
    return 0;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

BOOLEAN_RETURN uint8_t GBRunLoopIsRunning(const GBRunLoop* rl)
{
    return rl->_running;
}



BOOLEAN_RETURN uint8_t GBRunLoopRun( GBRunLoop* rl)
{
    if( rl)
    {
        rl->_running = 1;
        rl->_shouldStop = 0;
        
        rl->_runningThread_id = pthread_self();
        
        while (rl->_shouldStop == 0 )
        {
            /*GBTimeMS timeSpent = */Internal_GBRunLoopRunOnce(rl);
            
        }
        rl->_running = 0;

        
        return 1;
    }
    
    return 0;
}

/*
void GBRunLoopRunOnce( GBRunLoop* rl)
{
    if (rl)
    {
        
    }
}
 */

BOOLEAN_RETURN uint8_t GBRunLoopStop( GBRunLoop* rl)
{
    if( rl)
    {
        if(rl->_running)
        {
            rl->_shouldStop = 1;
            
            //SECRET_PIPE_BYTE2
            const uint8_t b = SECRET_PIPE_BYTE2;
            if(write(rl->_asyncPipeFDs[1], &b , 1) != 1)
            {
                DEBUG_ASSERT(0);
            }
            
            
            return 1;
        }
    }
    return 0;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */


static int Internal_GBRunLoopGetTimeToWait(GBRunLoop* self)
{
#ifdef USE_TIMER_FD
    return -1;
#else 
    return (int) TimerWheelGetTimeout(self->_timersWheel);
#endif
}

static GBTimeMS Internal_GBRunLoopRunOnce(GBRunLoop* self)
{
    
    GBTimeMS timeSpent = Internal_GBRunLoopPoll(self);
    
    Internal_GBRunLoopUpdateTimers( self , timeSpent );
    /*
    if( GBRunLoopGetNumSources(self) == 0)
    {
        
    }
    */
    return timeSpent;
}

static GBTimeMS Internal_GBRunLoopPoll(GBRunLoop* self)
{
    
    DEBUG_ASSERT(self);
    DEBUG_ASSERT(self->_asyncCalls);
    DEBUG_ASSERT(self->_fdSources);
    DEBUG_ASSERT(self->_running);
    DEBUG_ASSERT(self->_shouldStop == 0);
    
    const GBSize numAsyncPipe = 1;

    const GBSize numFDSources = GBRunLoopGetNumFDSources(self)
#ifdef USE_TIMER_FD
                              + GBRunLoopGetNumTimers(self)
#endif
                              ;
    
    const GBSize numSourcesTotal = numAsyncPipe + numFDSources;
    
    struct pollfd ufds[numSourcesTotal];
    
    memset(ufds, 0, sizeof(struct pollfd)*numSourcesTotal);
    /*
        pollfd layout:
            [0]      async pipe
            [1 ...n] sources
	    [n+1 ... nT] timers (if Linux target)
     */
    
    // async pipe
    ufds[0].fd = self->_asyncPipeFDs[0];
    ufds[0].events = POLLIN | POLLPRI;
    ufds[0].revents = 0;
    
    // fd sources
    const GBIndex indexPos = 1;
    GBIndex i = 0;
    GBRef source = NULL;
    GBListForEach(self->_fdSources, source)
    {
        ufds[indexPos+i].fd =AbstractFileDescriptorSourceGetFD( source );
        ufds[indexPos+i].events =     POLLIN
                                    | POLLPRI
                                    | POLLERR
                                    | POLLHUP
                                    | POLLNVAL;
        
        ufds[indexPos+i].revents = 0;
        i++;
    }

    DEBUG_ASSERT(i ==  GBRunLoopGetNumFDSources(self) );

#ifdef USE_TIMER_FD

    GBListForEach(self->_timers, source)
    {
        if( GBTimerIsActive(source) == 0)
        {
            continue;
        }
	
        //struct itimerspec timeRemaining;
        //timerfd_gettime( AbstractFileDescriptorSourceGetFD( source ) ,  &timeRemaining);

	
        ufds[indexPos+i].fd =AbstractFileDescriptorSourceGetFD( source );
        ufds[indexPos+i].events = POLLIN;
        ufds[indexPos+i].revents = 0;
        
        i++;
    }
#endif
    DEBUG_ASSERT(i <= numFDSources); // can be < if some timers are not active ( on Linux targets ONLY) 

    
    const int timeout = Internal_GBRunLoopGetTimeToWait(self);
    
    struct timeval start = (struct timeval){ 0 , 0};
    Internal_clock_gettime(&start);
    

    nfds_t realNumSourceTotal = (nfds_t) ++i;
    int ret = poll(ufds ,realNumSourceTotal, timeout );
    
    if (ret == -1)
    {
        PERROR("poll"); // error occurred in poll()
    }
    else if (ret != 0)
    {
        if ((ufds[0].revents & POLLIN) ) // Async calls
        {
            uint8_t b = 0;
            read(self->_asyncPipeFDs[0] ,&b , 1 );
            
            DEBUG_ASSERT(b == SECRET_PIPE_BYTE || b == SECRET_PIPE_BYTE2);
            if( b == SECRET_PIPE_BYTE)
            {
                Internal_GBRunLoopHandleAsyncCalls(self);
            }
            else if( b == SECRET_PIPE_BYTE2) // wake up, no call
            {
                // maybe return here?

            }
        }
        
        for ( i = 0;  i<realNumSourceTotal - 1; i++)
        {
            const GBIndex idFd = i+1;
            
            AbstractFileDescriptorSource* input = NULL;
            source = NULL;
            GBListForEach(self->_fdSources, source)
            {
                AbstractFileDescriptorSource* soc = CONST_CAST(AbstractFileDescriptorSource* ) source; // CONST_CAST is needed because we use GBListForEach on source.
                if( soc->_fd == ufds[idFd].fd)
                {
                    input = soc;
                    break;
                }
            }
            if (input)
            {
                Internal_CheckSource( self , input , &ufds[idFd]);
            }
#ifdef USE_TIMER_FD
            else //if( input == NULL)
            {

                GBListForEach(self->_timers, source)
                {
                    AbstractFileDescriptorSource* soc = CONST_CAST(AbstractFileDescriptorSource* ) source; // CONST_CAST is needed because we use GBListForEach on source.
                    if( soc->_fd == ufds[idFd].fd && ufds[idFd].revents & POLLIN )
                    {
                        DEBUG_ASSERT( GBTimerIsActive(source));
                    
                        uint64_t v = 0;
                        if( read ( soc->_fd  , &v , sizeof(uint64_t) ) )
                        {
                            AbstractRunLoopSourceNotify( (AbstractRunLoopSource*) soc, GBRunLoopSourceTimerFired );
                        }
                        else
                        {
                            PERROR("Timer.read");
                            DEBUG_ASSERT(0); // to also investigate :)
                        }
                    
                        break;
                    }
                }
            
            } // END else //if( input == NULL)
#endif
        }
        
    }
    
    
    struct timeval end = (struct timeval){ 0 , 0};
    Internal_clock_gettime(&end);
    
    struct timeval deltaT;
    timersub(&end, &start, &deltaT);
    
    const GBTimeMS timeSpentMS =  (GBTimeMS)((deltaT.tv_sec * 1000 )+ (deltaT.tv_usec / 1000));
    
    
    return timeSpentMS;
}

static BOOLEAN_RETURN uint8_t Internal_CheckSource( GBRunLoop* runLoop , AbstractFileDescriptorSource* source , struct pollfd *fd)
{
    DEBUG_ASSERT(runLoop);
    DEBUG_ASSERT(source);
    DEBUG_ASSERT(fd);
    
    if( runLoop && source && fd)
    {
        if(   fd->revents & POLLERR)
        {
            
            source->source._callback(source, GBRunLoopSourceError );
            //GBrunLoopNotify(source, GBRunLoopSourceErrorRead );
            //GBRunLoopRemoveSource(runLoop, source);
            return 1;
        }
        if(fd->revents & POLLHUP)
        {
            source->source._callback(source, GBRunLoopSourceDisconnected );
            
            //GBRunLoopRemoveSource(runLoop, source);
            return 1;
        }
        if(fd->revents & POLLNVAL)
        {
            DEBUG_LOG("[GBRunLoop] Source read error POLLNVAL \n");
        }
        else if (( fd->revents & POLLIN) || ( fd->revents & POLLPRI)  )
        {
            source->source._callback(source , GBRunLoopSourceCanRead);
        }
    }
    return 0;
}

static BOOLEAN_RETURN uint8_t Internal_GBRunLoopHandleAsyncCalls(GBRunLoop* self)
{
    DEBUG_ASSERT(self);

    GBRunLoopLock(self);
    
    uint8_t ret= 0;
    if( ArrayGetSize( self->_asyncCalls ) > 0)
    {
        AsyncCall* task = CONST_CAST(AsyncCall*) ArrayGetValueAtIndex(self->_asyncCalls, 0);
        
        ArrayRemoveValueAtIndex(self->_asyncCalls, 0);
        
        if(task)
        {
            Internal_GBRunLoopInvokeCall(self , task);
            
            
            //ret = ArrayRemoveValueAtIndex(self->_asyncCalls, 0);
            
            GBFree( task );
        }
    }
    else 
    {
	    DEBUG_LOG("Internal_GBRunLoopHandleAsyncCalls async calls queue is empty! ");
    }
    
    GBRunLoopUnlock(self);
    return ret;
    
}

BOOLEAN_RETURN uint8_t Internal_clock_gettime( struct timeval *tv)
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


static void Internal_GBRunLoopUpdateTimers(GBRunLoop *self  , GBTimeMS timeSpent )
{
#ifndef USE_TIMER_FD

    TimerWheelStep( self->_timersWheel,  timeSpent/* timeToWait*/ );
    
    struct timeout *timerIMP = NULL;
    
    while ( ( timerIMP = TimerWheelGetFiredTimers( self->_timersWheel ) ) != NULL )
    {
        GBTimer* realTimer = TimerGetUserContext( timerIMP );
        DEBUG_ASSERT( realTimer);
        
        DEBUG_ASSERT( realTimer->_changed == 0);
        
        AbstractRunLoopSourceNotify( &realTimer->super, GBRunLoopSourceTimerFired );
        
    }

#endif
    GBRef value = NULL;
    GBListForEach(self->_timers, value)
    {
        GBTimer* timer = CONST_CAST(GBTimer* ) value; // CONST_CAST is needed because we use GBListForEach on value.
        if( timer->_changed)
        {
            Internal_GBTimerUpdateChanges( self, timer );
        }
    }
}



BOOLEAN_RETURN uint8_t  GBRunLoopLock( GBRunLoop* rl)
{
    if (rl == NULL)
        return 0;
    return pthread_mutex_lock( &rl->_lock ) == 0;
}

BOOLEAN_RETURN uint8_t  GBRunLoopTryLock( GBRunLoop*  rl)
{
    if (rl == NULL)
        return 0;
    
    return pthread_mutex_trylock( &rl->_lock ) == 0;
}

BOOLEAN_RETURN uint8_t  GBRunLoopUnlock( GBRunLoop*  rl)
{
    if (rl == NULL)
        return 0;
    
    return pthread_mutex_unlock( &rl->_lock ) == 0;
}
