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
//  GBRunLoop.h
//  GroundBase
//
//  Created by Manuel Deneu on 31/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

/**
 * \file GBRunLoop.h
 * \brief The RunLoop API.
 *
 * GBRunLoop : C object that handles sources such as timers or sockets an provides a event driven model. See GBRunLoop.hpp for C++ API.
 */

#ifndef GBRunLoop_h
#define GBRunLoop_h

#include <GBObject.h>
#include <GBRunLoopSource.h>
#include <GBTimer.h>

GB_BEGIN_DCL

extern GBObjectClassRef GBRunLoopClass;
#define GBRunLoopClassName (const char*) "GBRunLoop"

/*!
 * @discussion A GBRunLoop source opaque object. Create an instance using `GBRunLoopInit` and release with `GBRelease`.
 */
typedef struct _GBRunLoop GBRunLoop;

/*!
 * @discussion Creates a GBRunLoop instance
 * @return A new GBRunLoop instance, in a not running state.
 */
GBRunLoop* GBRunLoopInit( void );

/*!
 * @discussion Runs the runloop until `GBRunLoopStop` is called.
 * @param rl a runloop instance
 * @return 0 if rl is NULL, or block until `GBRunLoopStop` is called and will return 1.
 */
BOOLEAN_RETURN uint8_t GBRunLoopRun( GBRunLoop* rl);
    
/*!
 * @discussion Stops the runloop. This is safe to call this method from any other thread.
 * @param rl a runloop instance
 * @return 0 if rl is NULL or the runloop is not running, 1 on sucess. Note that the stop might be async, and thus not immediate on return.
 */
BOOLEAN_RETURN uint8_t GBRunLoopStop( GBRunLoop* rl);

GB_DEPRECATED("5.3.7") BOOLEAN_RETURN uint8_t GBRunLoopIsRunning(const GBRunLoop* rl);

/* Sources */
    
/*!
 * @discussion Returns the number of added timers, active or not.
 * @param rl a runloop instance
 * @return the number of added timers.
 */
GBSize GBRunLoopGetNumTimers( const GBRunLoop* rl);
    
/*!
 * @discussion Returns the number of added file descriptor source.
 * @param rl a runloop instance
 * @return the number of added file descriptor.
 */
GBSize GBRunLoopGetNumFDSources( const GBRunLoop* rl);
    
/*!
 * @discussion Returns the total number of sources, ie `GBRunLoopGetNumTimers + GBRunLoopGetNumFDSources`
 * @param rl a runloop instance
 * @return the total number of sources.
 */
GBSize GBRunLoopGetNumSources( const GBRunLoop* rl);

/*!
 * @discussion Adds a source for the runloop. Note : the source must have been removed from any previous runloop or indefined behaviour will occur.
 * @param rl a runloop instance
 * @param source a runloop source.
 * @return 0 if rl or source is NULL, 1 on sucess.
 */
BOOLEAN_RETURN uint8_t GBRunLoopAddSource( GBRunLoop* rl , GBRunLoopSource* source);

/*!
 * @discussion Removes a source for the runloop.
 * @param rl a runloop instance
 * @param source a runloop source.
 * @return 0 if rl, or source is NULL, or the source does not belong the this runloop, 1 on sucess.
 */
BOOLEAN_RETURN uint8_t GBRunLoopRemoveSource( GBRunLoop* rl , GBRunLoopSource* source);

/*!
 * @discussion Checks if a source belongs to a runloop.
 * @param runLoop a runloop instance
 * @param source a runloop source.
 * @return 0 if rl, or source is NULL, or the source does not belong the this runloop, 1 if the source belongs to the runloop rl.
 */
BOOLEAN_RETURN uint8_t GBRunLoopContainsSource(GBRunLoop* runLoop , const GBRunLoopSource*source);
    
GB_DEPRECATED("5.3.6") void  GBRunLoopSetUserContext(GBRunLoop* runLoop , void* context );
GB_DEPRECATED("5.3.6") void* GBRunLoopGetUserContext( const GBRunLoop* runLoop);

/* Dispatch*/

/*!
 * @discussion The general callback form to pass to GBRunLoopDispatchAsync/GBRunLoopDispatchAfter
 */
typedef void (*GBRunLoopAsyncCallback)(GBRunLoop* runLoop , void* data);

    
/*!
 * @discussion Executes a callback asynchronously on a runloop.
 * @param runLoop a runloop instance
 * @param callback a method to call. Must have a signature following 'GBRunLoopAsyncCallback' convention.
 * @param data a user pointer passed to the callback.
 * @return 0 if rl is NULL, 1 if the call suceeds.
 */
BOOLEAN_RETURN uint8_t GBRunLoopDispatchAsync( GBRunLoop* runLoop , GBRunLoopAsyncCallback callback, void* data);
    
/*!
 * @discussion Executes a callback asynchronously on a runloop, after a certain delay.
 * @param runLoop a runloop instance
 * @param callback a method to call. Must have a signature following 'GBRunLoopAsyncCallback' convention.
 * @param data a user pointer passed to the callback.
 * @param waitTime the time to wait before actualy invoking the callback. Not that his time is approximate, and might take a little longer than expected.
 * @return 0 if rl is NULL, 1 if the call suceeds.
 */
BOOLEAN_RETURN uint8_t GBRunLoopDispatchAfter( GBRunLoop* runLoop , GBRunLoopAsyncCallback callback, void* data , GBTimeMS waitTime);

GB_END_DCL
    
#endif /* GBRunLoop_h */
