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
//  GBrunLoopSource.h
//  GroundBase
//
//  Created by Manuel Deneu on 29/10/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#ifndef ABSTRACTRUNLOOPSOURCE_h
#define ABSTRACTRUNLOOPSOURCE_h


#include <GBRunLoop.h>
#include <GBRunLoopSource.h>
#include "../GBObject_Private.h"



/*
 Note : this struct is public inside GroundBase ( but not visible for users) for allocation purposes. 
 You should not access its fields directly, uses getters bellow.
 */
struct _AbstractRunLoopSource
{
    GBObjectBase base;      // always first!
    GBRunLoopSourceCallback _callback;
    GBRunLoop*              _currentRunLoop; // just a ref!
    void*                   _userContext;
    
} ;
typedef struct _AbstractRunLoopSource AbstractRunLoopSource;



BOOLEAN_RETURN uint8_t AbstractRunLoopSourceInit( AbstractRunLoopSource* self , GBRunLoopSourceCallback callback);

// runLoop can be NULL
void AbstractRunLoopSourceSetRunLoop(AbstractRunLoopSource* self , GBRunLoop* runLoop);

void AbstractRunLoopSourceNotify( AbstractRunLoopSource* source , GBRunLoopSourceNotification reason);


#endif /* ABSTRACTRUNLOOPSOURCE_h */
