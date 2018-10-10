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
//  GBRunLoop_Private.h
//  GroundBase
//
//  Created by Manuel Deneu on 21/11/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#ifndef GBRunLoop_Private_h
#define GBRunLoop_Private_h

#include <pthread.h>

#include <GBList.h>

#include "GBObject_Private.h"
#include "AsyncCall.h"


struct _GBRunLoop
{
    GBObjectBase    base;
    //pthread_cond_t  condWait;
    uint8_t         _shouldStop;
    uint8_t         _running;
    
    pthread_mutex_t _lock;
    pthread_t   _runningThread_id;
    
    GBList*      _fdSources;
    GBList*      _timers;
    
    Array *     _asyncCalls;
    int         _asyncPipeFDs[2];
    
    
    void *_userContext;
#ifndef USE_TIMER_FD
    void* _timersWheel;
#endif

};

void Internal_GBRunLoopInvokeCall(GBRunLoop* self , AsyncCall *task);


BOOLEAN_RETURN uint8_t  GBRunLoopLock( GBRunLoop* rl);
BOOLEAN_RETURN uint8_t  GBRunLoopTryLock( GBRunLoop*  rl);
BOOLEAN_RETURN uint8_t  GBRunLoopUnlock( GBRunLoop*  rl);

#endif /* GBRunLoop_Private_h */
