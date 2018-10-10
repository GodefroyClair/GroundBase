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
//  GBRunLoopDispatch.c
//  GroundBase
//
//  Created by Manuel Deneu on 25/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//


#include <unistd.h> // write
#include <stdlib.h>
#include <GBRunLoop.h>
#include "GBRunLoop_Private.h"



static BOOLEAN_RETURN uint8_t Internal_GBRunLoopAddAsyncCall(GBRunLoop* runloop , AsyncCall *call);


GB_HOT BOOLEAN_RETURN uint8_t GBRunLoopDispatchAsync( GBRunLoop* runLoop , GBRunLoopAsyncCallback callback, void* data)
{
    if( runLoop == NULL || callback == NULL)
        return 0;
    
    uint8_t shouldUnlock = 0;
    if( runLoop->_runningThread_id != pthread_self())
    {
        shouldUnlock = 1;
        GBRunLoopLock( runLoop);
        
    }
    
    uint8_t ret = 0;
    AsyncCall* newCall = AsyncCallInit();
    if( newCall)
    {
        newCall->_callback = callback;
        newCall->_userData = data;
        

        const uint8_t b = SECRET_PIPE_BYTE;
        
        if(write(runLoop->_asyncPipeFDs[1], &b , 1) != 1)
        {
            DEBUG_ASSERT(0);
        }
        ret = Internal_GBRunLoopAddAsyncCall (runLoop , newCall);
        
    }

    if( shouldUnlock)
    {
        GBRunLoopUnlock(  runLoop);
    }
    return ret;
}

/* **** **** **** **** **** **** **** **** */

static void Internal_invokeAfter( GBRunLoopSource* source , GBRunLoopSourceNotification notification)
{
    UNUSED_PARAMETER(notification);
    DEBUG_ASSERT(notification == GBRunLoopSourceTimerFired);
    
    DEBUG_ASSERT(source);
    AsyncCall* task = GBRunLoopSourceGetUserContext(source);
    DEBUG_ASSERT(task);
    
    GBRunLoop* rl = GBRunLoopSourceGetRunLoop(source);
    DEBUG_ASSERT(rl);
    task->_callback( rl , task->_userData);
    
    GBRunLoopRemoveSource(rl, source);
    
    
    GBFree(task);
}

BOOLEAN_RETURN uint8_t GBRunLoopDispatchAfter( GBRunLoop* runLoop , GBRunLoopAsyncCallback callback, void* data , GBTimeMS waitTime)
{
    if( runLoop == NULL || callback == NULL )
        return 0;
    
    AsyncCall* newCall = AsyncCallInit();
    if( newCall)
    {
        newCall->_callback = callback;
        newCall->_userData = data;
    
        GBTimer* t = GBTimerInit(Internal_invokeAfter);
        DEBUG_ASSERT(t);
        GBTimerSetIntervalMS(t, waitTime);
        GBTimerSetPeriodic(t, 0);
        GBRunLoopSourceSetUserContext(t,newCall);
        const uint8_t ret = GBRunLoopAddSource( runLoop, t);
        
        
        GBRelease(t);
        
        return ret;
        
    }
    return 0;
    
}

static BOOLEAN_RETURN uint8_t Internal_GBRunLoopAddAsyncCall(GBRunLoop* runloop , AsyncCall *call)
{
    // GBRunLoop will already locked if called from an other thread than the runloop's.
    return ArrayAddValue(runloop->_asyncCalls, call);
}

void Internal_GBRunLoopInvokeCall(GBRunLoop* self , AsyncCall *task)
{
    DEBUG_ASSERT(self);
    DEBUG_ASSERT(task);
    
    task->_callback(self , task->_userData);
    
    
    
}
