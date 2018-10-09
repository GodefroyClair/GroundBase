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
//  GBObject_Private.h
//  GroundBase
//
//  Created by Manuel Deneu on 28/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#ifndef GBObject_Private_h
#define GBObject_Private_h

#include <pthread.h>
#include "../include/GBObject.h"
#include "Private/List.h"
#include "Private/Array.h"
#include "Private/Dictionary.h"
#include <GBString.h>
#include "GBAllocator.h"


#define GBRUNTIME_CTOR_PRIORITY (int) 100

typedef void* (* GBObjectConstructorCallback) (void * self, va_list * app) ;
typedef void* (* GBObjectDestructorCallback) (void * self);
typedef void* (* GBObjectCloneCallback) (const void * self);
typedef uint8_t  (* GBObjectCloneEqualityCallback) (const void * self, const void * b);

typedef void (* GBObjectClassInitializeCallback)(void);
typedef void (* GBObjectClassDeInitializeCallback)(void);


typedef GBObject GBCoder;
//typedef uint8_t (* GBObjectSerializeCallback)(const void * _self ,GBCoder* coder , const GBString* key );

typedef GBRef (* GBObjectDescriptionCallback) (const void * self);

typedef void (*GBObjectRetainCallback)( GBRef _self);
typedef void (*GBObjectReleaseCallback)( GBRef _self);

struct _GBObjectClass
{
    
    GBSize size;
    GBObjectConstructorCallback       constructor;
    GBObjectDestructorCallback        destructor;
    GBObjectCloneCallback             clone;
    GBObjectCloneEqualityCallback     equals;
    GBObjectDescriptionCallback       description;
    GBObjectClassInitializeCallback   initialize;
    GBObjectClassDeInitializeCallback deInitialize;
    //GBObjectSerializeCallback         serialize;
    
    GBObjectRetainCallback  retain;
    GBObjectReleaseCallback release;
    char* name;
};

typedef enum
{
    GBObjectUninitialized = 0,
    GBObjectValid = 1,
    GBObjectFreed = 2,
} GBObjectState;

/*
 You must inherit from this structure to create new GBObjects
 */
typedef struct _GBObjectBase
{
    GBObjectClass *class; /* Always first !! */

    GBAllocator _allocator;
    
    int refCount;
    GBObjectState state;

    Array* strongReferences;

    pthread_mutex_t _lock;

}GBObjectBase;

/*
 The GBObject allocator :
 
 Will set retain count to 1
 */

void *GBObjectAlloc( GBAllocator allocator , const void * class, ...);


/* GBObject life cycle managment */

// Will retain child
GB_DEPRECATED("5.4.3") BOOLEAN_RETURN uint8_t GBObjectAddStrongReference( GBRef object ,  GBRef  child );

GBObjectState GBObjectGetState(GBRef object);

pthread_mutex_t* GBObjectGetMutex( GBRef object);


const char* GBObjectGetClassNameC( GBRef object);
/*
 Defined in GBString.c. GBRuntime needs this so Static Strings won't be tracked.
 */
extern GBAllocator StaticStringAllocator;


#endif /* GBObject_Private_h */
