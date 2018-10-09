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
//  GBThread.c
//  GroundBase
//
//  Created by Manuel Deneu on 11/11/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//


#include <pthread.h>
#include <GBThread.h>
#include <unistd.h>
#if defined(__linux__)
#include <sys/prctl.h>
#endif

#include "GBObject_Private.h"

static void InternaL_ThreadDidStop( GBThread* self);
static void InternaL_ThreadStart( GBThread* self);

static void * ctor(void * _self, va_list * app);
static void * dtor (void * _self);

static uint8_t  equals (const void * _self, const void * _b);
static GBRef description (const void * self);

struct _GBThread
{
    GBObjectBase base;
    
    GBThreadMain _main;
    
    uint8_t _isRunning;
    uint8_t _shouldQuit;
    
    pthread_t thread_id;
    pthread_cond_t  condWait;
    

    pthread_cond_t  _condInitialized; // signal() 'ed after thread is spawned
    
    GBString *name;
    void* _userContext;
};

static const GBObjectClass _GBThreadClass =
{
    sizeof(struct _GBThread ),
    ctor,
    dtor,
    NULL, // clone
    equals,
    description,
    NULL, // initialize
    NULL, // deinit
    NULL, // retain
    NULL, // release
    (char*)GBThreadClassName
};

GBObjectClassRef GBThreadClass = & _GBThreadClass;

static void * ctor(void * _self, va_list * app)
{
    UNUSED_PARAMETER( app );
    GBThread* self = _self;
    
    if( self)
    {
        self->thread_id  = NULL;
        self->_shouldQuit = 0;
        self->_isRunning = 0;
        self->_userContext = NULL;
        self->name = NULL;
        pthread_cond_init( &self->condWait , NULL);
        pthread_cond_init( &self->_condInitialized , NULL);

        
        return self;
    }
    return NULL;
}

static void * dtor (void * _self)
{
    GBThread* self = _self;
    if( self)
    {
        pthread_cond_destroy( &self->condWait );
        pthread_cond_destroy( &self->_condInitialized );
        
        if( self->name)
        {
            GBRelease(self->name);
        }
        return self;
    }
    return NULL;
}

static uint8_t  equals (const void * _self, const void * _b)
{
    const GBThread* a = _self;
    const GBThread* b = _b;
    
    if ( a && b)
    {
        return pthread_equal( a->thread_id , b->thread_id) != 0;
    }
    return 0;
}
static GBRef description (const void * _self)
{
    const GBThread* self = (const GBThread*) _self;
    if( self)
    {
        return GBObjectClone( self->name);
    }
    
    return NULL;
}


GBThread* GBThreadInit()
{
    GBThread* self = GBObjectAlloc( GBDefaultAllocator,GBThreadClass);
    
    if( self)
    {
        return self;
    }
    
    return NULL;
}

BOOLEAN_RETURN uint8_t GBThreadSetName( GBThread* thread,const GBString* name)
{
    if( thread && GBStringIsValid(name))
    {
        if( thread->name)
        {
            GBRelease(thread->name);
            thread->name = NULL;
        }
        thread->name = GBObjectClone(name);
        

        return thread->name != NULL;
    }
    return 0;
}
const GBString* GBThreadGetName( const GBThread* thread)
{
    if( thread )
    {
        return thread->name;
    }
    
    return NULL;
}

BOOLEAN_RETURN uint8_t GBThreadSetMain( GBThread* thread , GBThreadMain mainFunction )
{
    if( thread == NULL )
        return 0;
    
    thread->_main = mainFunction;
    return 1;
}
static void Internal_setThreadName( const char*name)
{
#if defined(__linux__)
    /* Use prctl instead to prevent using _GNU_SOURCE flag and implicit declaration */
    prctl(PR_SET_NAME, name );
#elif defined(__APPLE__) && defined(__MACH__)
    pthread_setname_np( name );
#else
    DEBUG_LOG("[GBThreadSetName] pthread_setname_np is not supported on this system");
#endif
}
static void InternaL_ThreadStart( GBThread* self)
{
    DEBUG_ASSERT( self);
    

    if( self->name)
    {
        Internal_setThreadName( GBStringGetCStr( self->name));
    }
    GBObjectLock(self);
    self->_isRunning = 1;
    pthread_cond_signal(  &self->_condInitialized );
    GBObjectUnlock(self);
    

    self->_main(self);
    
    InternaL_ThreadDidStop( self );
}

static void InternaL_ThreadDidStop( GBThread* self)
{
    self->_isRunning = 0;
}

BOOLEAN_RETURN uint8_t GBThreadStart( GBThread* thread )
{
    
    if( thread == NULL )
        return 0;
    
    if( thread->_main == NULL)
        return 0;
    
    if( GBThreadIsRunning(thread))
    {
        return 0;
    }
    
    thread->_shouldQuit = 0;
    
    const int ret = pthread_create ( &thread->thread_id, NULL,(void *(*)(void *)) InternaL_ThreadStart, thread ) ;
    
    if( ret != 0 )
    {
        PERROR("GBThreadStart] pthread_create ");
    }
    
        
    return ret == 0;
}

BOOLEAN_RETURN uint8_t GBThreadWaitForStart( GBThread* thread )
{
    if( thread == NULL )
        return 0;
    
    GBObjectLock( CONST_CAST(GBThread*) thread);
    
    while ( thread->_isRunning == 0 )
    {
        pthread_cond_wait( &thread->_condInitialized, &thread->base._lock);
        
    }
    
    GBObjectUnlock( CONST_CAST(GBThread*) thread);
    return 1;
}

void GBThreadSendAsyncStop( GBThread* thread)
{
    if( thread == NULL)
        return ;

    thread->_shouldQuit = 1;
    
    GBThreadWake( thread);
}

BOOLEAN_RETURN uint8_t GBThreadWaitForTerminaison( const GBThread *thread )
{
    if( thread == NULL)
        return 0;
    
    return pthread_join ( thread->thread_id, NULL) == 0? 1 : 0;
}

BOOLEAN_RETURN uint8_t GBThreadIsRunning( const GBThread *thread )
{
    if (thread == NULL)
        return 0;
    //GBObjectLock(thread);
    return  thread->_isRunning;// && thread->thread_id != 0;
}

BOOLEAN_RETURN uint8_t GBThreadShouldReturn( const GBThread *thread )
{
    if (thread == NULL)
        return 0;
    
    return  thread->_shouldQuit;
}

void GBThreadSetUserContext( GBThread* thread , void* userPtr)
{
    if (thread == NULL)
        return ;
    
    thread->_userContext = userPtr;
}
void* GBThreadGetUserContext( const GBThread* thread )
{
    if (thread == NULL)
        return NULL;
    
    return thread->_userContext;
}


BOOLEAN_RETURN uint8_t GBThreadWaitForever( GBThread* thread )
{
    if (thread == NULL)
        return 0;
    
    return pthread_cond_wait(&thread->condWait, &thread->base._lock) == 0;
}

BOOLEAN_RETURN uint8_t GBThreadWaitForMS( GBThread* thread , GBTimeMS timeout )
{
    if (thread == NULL)
        return 0;
    
    struct timespec t;
    t.tv_nsec = (timeout % 1000)*1000;
    t.tv_sec = (timeout / 1000);
    
    return pthread_cond_timedwait(&thread->condWait, &thread->base._lock, &t) == 0;
}

BOOLEAN_RETURN uint8_t GBThreadWake( GBThread* thread )
{
    return pthread_cond_broadcast( &thread->condWait) == 0;
}

BOOLEAN_RETURN uint8_t GBThreadCalledFromThis( const GBThread* thread)
{
    if (thread == NULL)
        return 0;
    
    return pthread_equal(pthread_self(), thread->thread_id) != 0;
}
