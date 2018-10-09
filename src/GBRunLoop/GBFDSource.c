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
//  GBFDSource.c
//  GroundBase
//
//  Created by Manuel Deneu on 12/11/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#include <string.h>
#include <unistd.h> // close
#include <GBFDSource.h>

#include "../GBObject_Private.h"

#include "AbstractFileDescriptorSource.h"



static void * GBFDSource_ctor(void * _self, va_list * app);
static void * GBFDSource_dtor (void * _self);
static uint8_t  GBFDSource_equals (const void * _self, const void * _b);
static GBRef GBFDSource_description (const void * self);

static const GBObjectClass _GBFDSourceClass =
{
    sizeof(struct _AbstractFileDescriptorSource),
    GBFDSource_ctor,
    GBFDSource_dtor,
    NULL,//RunLoop_clone,
    GBFDSource_equals,
    GBFDSource_description,
    NULL, // initialize
    NULL, // deinit
    NULL,
    NULL,
    (char*)GBFDSourceClassName
};

GBObjectClassRef GBFDSourceClass = & _GBFDSourceClass;

static void * GBFDSource_ctor(void * _self, va_list * app)
{
    GBFDSource *self = _self;
    
    if( self)
    {
        GBRunLoopSourceCallback callback = va_arg(*app, GBRunLoopSourceCallback);
        
        if( AbstractFileDescriptorSourceInit( self , callback))
        {
            return self;
        }
    }
    
    return NULL;
}
static void * GBFDSource_dtor (void * _self)
{
    GBFDSource *self = _self;
    
    if( self)
    {
        if( self->closeOnDestruct)
        {
            close(self->_fd);
        }
    }
    
    return self;
}
static uint8_t  GBFDSource_equals (const void * _self, const void * _b)
{
    const GBFDSource* a = _self;
    const GBFDSource* b = _b;
    
    if (a && b)
    {
        return a->_fd == b->_fd;
    }
    return 0;
}
static GBRef GBFDSource_description (const void * _self)
{
    const GBFDSource* self = (const GBFDSource* ) _self;
    
    if( self)
    {
        return GBStringInitWithFormat("fd num %i" , self->_fd);
        
    }
    
    return NULL;
}



GBFDSource* GBFDSourceInitWithFD( int fd , GBRunLoopSourceCallback callback )
{
    GBFDSource* self = GBObjectAlloc( GBDefaultAllocator, GBFDSourceClass ,  callback);
    
    if( self)
    {
        self->_fd = fd;
        
        return self;
    }
    
    return NULL;
}

int GBFDSourceGetFileDescriptor( const GBFDSource* source)
{
    if( source)
    {
        return source->_fd;
    }
    
    return UNINITIALIZED_FD;
}

void GBFDSourceShouldCloseOnDestruct( GBFDSource* source  , uint8_t shouldClose)
{
    if( source)
    {
        source->closeOnDestruct = shouldClose == 0?0:1;
        
    }
}

GBSize GBFDSourceRead( GBFDSource* source , void* content , GBSize size)
{
    return AbstractFileDescriptorSourceRead(source, content, size);
}

GBSize GBFDSourceSend( GBFDSource* source , const void* data , GBSize dataLength ,int flags)
{
    return AbstractFileDescriptorSourceSend(source, data, dataLength, flags);
}

GBSize GBFDSourceWrite( GBFDSource* source , const void* data , GBSize dataLength )
{
    return AbstractFileDescriptorSourceWrite(source, data, dataLength);
}




