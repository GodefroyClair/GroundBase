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
//  AbstractFileDescriptorSource.h
//  GroundBase
//
//  Created by Manuel Deneu on 19/10/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#ifndef AbstractFileDescriptorSource_h
#define AbstractFileDescriptorSource_h

#include <GBRunLoopSource.h>

#include "../GBObject_Private.h"
#include "AbstractRunLoopSource.h"

#define UNINITIALIZED_FD (int) -1

struct _AbstractFileDescriptorSource
{
    AbstractRunLoopSource source;
    int _fd;
    uint8_t closeOnDestruct;
    
} ;

typedef struct _AbstractFileDescriptorSource AbstractFileDescriptorSource;


BOOLEAN_RETURN uint8_t AbstractFileDescriptorSourceInit(AbstractFileDescriptorSource* self , GBRunLoopSourceCallback callback);

int AbstractFileDescriptorSourceGetFD( const AbstractFileDescriptorSource* self);

//returns GBSizeInvalid on error
GBSize AbstractFileDescriptorSourceRead( AbstractFileDescriptorSource* self , void* content , GBSize size);

// will return 0 on error
GBSize AbstractFileDescriptorSourceSend( AbstractFileDescriptorSource *source , const void* data , GBSize dataLength , int flags);
GBSize AbstractFileDescriptorSourceWrite( AbstractFileDescriptorSource *source , const void* data , GBSize dataLength );


BOOLEAN_RETURN uint8_t AbstractFileDescriptorSourceReadBlock( AbstractFileDescriptorSource* source , void* content , GBSize size);
BOOLEAN_RETURN uint8_t AbstractFileDescriptorSourceWriteBlock( AbstractFileDescriptorSource *source , const void* data , GBSize dataLength);
BOOLEAN_RETURN uint8_t AbstractFileDescriptorSourceSendBlock( AbstractFileDescriptorSource *source , const void* data , GBSize dataLength , int flags);

#endif /* AbstractFileDescriptorSource_h */
