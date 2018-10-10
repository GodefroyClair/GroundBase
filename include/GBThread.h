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
//  GBThread.h
//  GroundBase
//
//  Created by Manuel Deneu on 11/11/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

/**
 * \file GBThread.h
 * \brief Mostly a wrapper around phtread API.
 */

#ifndef GBThread_h
#define GBThread_h


#include <GBCommons.h>
#include <GBObject.h>
#include <GBTypes.h>
#include <GBString.h>

GB_BEGIN_DCL

extern GBObjectClassRef GBThreadClass;

#define GBThreadClassName (const char*) "GBThread"

    
/*!
 * @discussion An opaque GBThread instance.
 */
typedef struct _GBThread GBThread;

    
/*!
 * @discussion The generic form for the thread entry point method to pass to `GBThreadSetMain`.
 */
typedef void (*GBThreadMain)( GBThread *self);

/*!
 * @discussion Initializes an empty GBThread instance. You own the returned object. See GBObject ownership notes.
 * @return an empty GBThread instance
 */
GBThread* GBThreadInit(void);

    
/*!
 * @discussion Sets the thread entry point method for a thread.
 * @param thread a valid GBThread instance. Will return 0 if NULL.
 * @param mainFunction the thread entry point method. See `GBThreadMain` signature. Undefined behaviour if NULL or invalid.
 * @return 0 on error, 1 on sucess.
 */
BOOLEAN_RETURN uint8_t GBThreadSetMain( GBThread* thread , GBThreadMain mainFunction );

/*!
 * @discussion Sets the thread's name. This must be called before running the thread with GBThreadStart.
 * @param thread a valid GBThread instance. Will return 0 if NULL.
 * @param name a valid GBString instance. See `GBStringIsValid`.
 * @return 1 on success, 0 if thread is NULL, the name is NULL or invalid ( see GBStringIsValid ).
 */
BOOLEAN_RETURN uint8_t GBThreadSetName( GBThread* thread , const GBString* name);

/*!
 * @discussion Starts the thread.
 * @param thread a valid GBThread instance. Will return 0 if NULL.
 * @return 1 on success, 0 if no entry point method was set before (see `GBThreadSetMain`), the thread is already running, or thread is NULL.
 */
BOOLEAN_RETURN uint8_t GBThreadStart( GBThread* thread );
    
/*!
 * @discussion Wait for the thread to be started. You can call this method right after `GBThreadStart`.
 * @param thread a valid GBThread instance. Will return 0 if NULL.
 * @return 1 on success, 0 if thread is NULL.
 */
BOOLEAN_RETURN uint8_t GBThreadWaitForStart( GBThread* thread );

/*!
 * @discussion returns the thread name. is NULL by default. See GBThreadSetName.
 * @param thread a valid GBThread instance. Will return NULL if NULL.
 * @return the thread's name, if set, NULL otherwise.
 */
const GBString* GBThreadGetName( const GBThread* thread);

/*!
 * @discussion Signals to the thread to stop. This method will return immediatly.
 * @param thread a valid GBThread instance. Has no effect if NULL
 */
void GBThreadSendAsyncStop( GBThread* thread);
    
/*!
 * @discussion Waits for the thread to stop.
 * @param thread a valid GBThread instance. Will return 0 if NULL.
 * @return 1 on success, 0 on error.
 */
BOOLEAN_RETURN uint8_t GBThreadWaitForTerminaison( const GBThread *thread );

BOOLEAN_RETURN uint8_t GBThreadIsRunning( const GBThread *thread );
BOOLEAN_RETURN uint8_t GBThreadShouldReturn( const GBThread *thread );

void GBThreadSetUserContext( GBThread* thread , void* userPtr);
void* GBThreadGetUserContext( const GBThread* thread );

BOOLEAN_RETURN uint8_t GBThreadWake( GBThread* thread);

BOOLEAN_RETURN uint8_t GBThreadWaitForever( GBThread* thread );
BOOLEAN_RETURN uint8_t GBThreadWaitForMS( GBThread* thread , GBTimeMS timeout );


BOOLEAN_RETURN uint8_t GBThreadCalledFromThis( const GBThread* thread);

BOOLEAN_RETURN uint8_t  GBThreadLock( GBThread* thread);
BOOLEAN_RETURN uint8_t  GBThreadTryLock( GBThread*  thread);
BOOLEAN_RETURN uint8_t  GBThreadUnlock( GBThread*  thread);
    
GB_END_DCL

#endif /* GBThread_h */
