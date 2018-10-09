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
//  GBTimer_Private.h
//  GroundBase
//
//  Created by Manuel Deneu on 25/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#ifndef GBTimer_Private_h
#define GBTimer_Private_h

#include "GBRunLoop/GBRunLoop_Private.h"
#include "GBRunLoop/AbstractRunLoopSource.h"

#ifdef USE_TIMER_FD
#include "GBRunLoop/AbstractFileDescriptorSource.h"
#endif

void Internal_GBTimerSetDirty( GBTimer* self);

BOOLEAN_RETURN uint8_t Internal_GBTimerUpdateChanges(GBRunLoop* runLoop, GBTimer* timer);

struct _GBTimer
{
#ifdef USE_TIMER_FD
    AbstractFileDescriptorSource super;
#else
    AbstractRunLoopSource super;
    void* _timerImpl;
#endif
    
    
    GBTimeMS _intervalMS;
    uint8_t _changed; // set to 1 if the next fields are changed
    uint8_t active;
    uint8_t _periodic;
    /*
     void* _timerImpl;
     long fireCount;
     
     
     struct timeval _interval;
     
     
     */
    
};


#endif /* GBTimer_Private_h */
