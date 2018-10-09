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
//  AbstractRunLoopSource.c
//  GroundBase
//
//  Created by Manuel Deneu on 25/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include "AbstractRunLoopSource.h"


BOOLEAN_RETURN uint8_t AbstractRunLoopSourceInit( AbstractRunLoopSource* self , GBRunLoopSourceCallback callback)
{
    DEBUG_ASSERT(self);
    
    if( self == NULL )
        return 0;
    
    self->_userContext = NULL;
    
    self->_callback = callback;
    self->_currentRunLoop = NULL;
    return 1;
}

void AbstractRunLoopSourceSetRunLoop(AbstractRunLoopSource* self , GBRunLoop* runLoop)
{
    DEBUG_ASSERT(self);
    //DEBUG_ASSERT(self->_currentRunLoop == NULL);// TODO  : add code to move source from one loop to another
    self->_currentRunLoop = runLoop;
}

GBObject* GBRunLoopSourceGetRunLoop(const GBRunLoopSource* source )
{
    DEBUG_ASSERT(source);
    
    const AbstractRunLoopSource* self = (const AbstractRunLoopSource*) source;
    if( self)
    {
        return self->_currentRunLoop;
    }
    
    return NULL;
}


void  GBRunLoopSourceSetUserContext(GBRunLoopSource* source , void* context )
{
    if( source)
    {
        AbstractRunLoopSource* self = (AbstractRunLoopSource*) source;
        self->_userContext = context;
    }
}
void* GBRunLoopSourceGetUserContext( const GBRunLoopSource* source)
{
    if( source)
    {
        return ((AbstractRunLoopSource*) source)->_userContext;
    }
    return NULL;
}


void AbstractRunLoopSourceNotify( AbstractRunLoopSource* source , GBRunLoopSourceNotification reason)
{
    DEBUG_ASSERT(source);
    source->_callback(source , reason);
}
