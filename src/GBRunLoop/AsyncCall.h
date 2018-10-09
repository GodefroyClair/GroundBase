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
//  AsyncCall.h
//  GroundBase
//
//  Created by Manuel Deneu on 14/11/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#ifndef AsyncCall_h
#define AsyncCall_h

#include <GroundBase.h>
#include <GBRunLoop.h>
#include "../GBAllocator.h"

// those defs are not intended to be _that_ secret. They're used to return from poll for dispatch and wake up.
#define SECRET_PIPE_BYTE  (uint8_t) 0x41
#define SECRET_PIPE_BYTE2 (uint8_t) 0x42 // wake up, no call

typedef struct
{
    GBRunLoopAsyncCallback _callback;
    void*                  _userData;
} AsyncCall;

static inline AsyncCall* AsyncCallInit()
{
    return GBMalloc(sizeof(AsyncCall) );
}

#endif /* AsyncCall_h */
