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
//  GBArray.c
//  GroundBase
//
//  Created by Manuel Deneu on 31/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//
#include <string.h>
#include <GBArray.h>

#include "../GBObject_Private.h"
#include "GBContainer_Private.h"
#include "../Private/Array.h"


static void * Array_ctor(void * _self, va_list * app);
static void * Array_dtor (void * _self);
static void * Array_clone (const void * _self);
static uint8_t  Array_equals (const void * _self, const void * _b);
static GBRef Array_description (const void * self);

static void retainCallback( GBRef _self);
static void releaseCallback( GBRef _self);

static GBContainerCallbacks _GBArrayCallbacks =
{
    (_GBContainerGetSize)        GBArrayGetSize,
    (_GBContainerAddValue)       GBArrayAddValue,
    (_GBContainerRemoveValue)    GBArrayRemoveValue,
    (_GBContainerContainsValue)  GBArrayContainsValue,
    (_GBContainerIterate)       GBArrayIterate
};


struct _GBArray
{
    GBContainerBase super;
    Array* _array;
    
};

static const GBObjectClass _ArrayClass =
{
    sizeof(struct _GBArray),
    Array_ctor,
    Array_dtor,
    Array_clone,
    Array_equals,
    Array_description,
    NULL, // initialize
    NULL, // deinit
    //Array_serialize,
    retainCallback,
    releaseCallback,
    (char*)GBArrayClassName
};


GBObjectClassRef GBArrayClass = & _ArrayClass;



static void * Array_ctor(void * _self, va_list * app)
{
    UNUSED_PARAMETER(app);
    GBArray *self = _self;
    
    if( self)
    {
        if( GBContainerBaseInit( &self->super, _GBArrayCallbacks ) )
        {
            self->_array = ArrayInit();
            return self;
        }
        
    }
    
    return NULL;
}

static void * Array_dtor (void * _self)
{
    GBArray* self = _self;
    
    GBArrayReleaseContent(self);
    ArrayFree( self->_array );
    self->_array = NULL;
    
    return self;
}

static void retainCallback( GBRef _self)
{
    UNUSED_PARAMETER(_self);
}

static void releaseCallback( GBRef _self)
{
    UNUSED_PARAMETER(_self);
}

static void * Array_clone (const void * _self)
{
    const GBArray* self = _self;
    
    if( self)
    {
        GBArray* dest = GBArrayInitWithCapacity(GBArrayGetCapacity(self));
        
        if( dest == NULL)
            return NULL;
        
        for (GBIndex i = 0; i <GBArrayGetSize(self); i++)
        {
            GBArrayAddValue(dest,   GBArrayGetValueAtIndex(self, i));
        }
        
        return dest;
    }
    return NULL;
}
static uint8_t  Array_equals (const void * _self, const void * _b)
{

    const GBArray* a = _self;
    const GBArray* b = _b;
    
    
    if( GBArrayGetSize(a) != GBArrayGetSize(b))
        return 0;
    
    
    
    for(GBIndex i = 0; i < GBArrayGetSize(a) ; i++)
    {
        if( GBObjectEquals( GBArrayGetValueAtIndex(a, i), GBArrayGetValueAtIndex(b, i)) == 0)
        {
            return 0;
        }
    }
    
    return 1;
}


static GBRef Array_description (const void * _self)
{
    const GBArray* self = (const GBArray*) _self;
    if( self)
    {
        return GBStringInitWithFormat("GBArray %zi" , GBArrayGetSize(self));
    }
    return NULL;
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GBArray* GBArrayInit(void)
{
    return GBObjectAlloc( GBDefaultAllocator , GBArrayClass);
}
GBArray* GBArrayInitWithCapacity(GBSize cap)
{
    return GBObjectAlloc( GBDefaultAllocator , GBArrayClass , cap);
    
}

BOOLEAN_RETURN uint8_t GBArraySetCapacity( GBArray* array , GBSize newCapacity)
{
    return ArraySetCapacity(array->_array, newCapacity);
}

GBSize GBArrayGetCapacity(const GBArray* array)
{
    return ArrayGetCapacity( array->_array );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    Methods conforming to GBCollectionType
 */

BOOLEAN_RETURN uint8_t GBArrayAddValue(GBArray* array , GBRef value)
{
    const uint8_t ret = ArrayAddValue( array->_array , value);
    if( ret)
    {
        GBRetain(value);
    }
    return ret;
}

BOOLEAN_RETURN uint8_t GBArrayContainsValue( const GBArray *array, const void* val)
{
    if( ArrayContainsValue( array->_array , val ))
        return 1;
    
    for (GBIndex i = 0; i < GBArrayGetSize(array) ; i++)
    {
        GBRef val2 = GBArrayGetValueAtIndex(array, i);
        
        if( GBObjectEquals(val, val2))
            return 1;
    }
    
    return 0;
}

GBSize GBArrayGetSize(const GBArray *array)
{
    return ArrayGetSize( array->_array );
}

GBRef GBArrayGetValueAtIndex( const GBArray *array, GBIndex pos)
{
    return ArrayGetValueAtIndex( array->_array, pos);
}

GBIndex GBArrayGetIndexForValue(const GBArray* array , const void* value)
{
    for (GBIndex i = 0; i < GBArrayGetSize( array ) ; i++)
    {
        if( GBObjectEquals( GBArrayGetValueAtIndex( array, i) , value))
        {
            return i;
        }
    }
    return GBIndexInvalid;
}



BOOLEAN_RETURN uint8_t GBArrayRemoveValueAtIndex( GBArray* array , GBIndex index)
{
    GBRef value = GBArrayGetValueAtIndex(array, index);
    if( value)
    {
        GBRelease(value);
    }
    return ArrayRemoveValueAtIndex( array->_array , index);
}

BOOLEAN_RETURN uint8_t GBArrayRemoveValue( GBArray* array , GBRef value)
{
    for (GBIndex i = 0; i < GBArrayGetSize(array) ; i++)
    {
        GBRef v = GBArrayGetValueAtIndex(array, i);
        if( GBObjectEquals( v , value ))
        {
            return GBArrayRemoveValueAtIndex(array , i);
        }
    }
    return 0;
}

BOOLEAN_RETURN uint8_t GBArrayClear( GBArray* array )
{
    GBArrayReleaseContent(array);
    return ArrayClear( array->_array );
}

BOOLEAN_RETURN uint8_t GBArraySetValueAt(GBArray *array, GBIndex pos, const void* val)
{
    return ArraySetValueAt(array->_array, pos, val);
}

void GBArrayReleaseContent(const GBArray *array)
{
    for (GBIndex i = 0; i <  GBArrayGetSize(array) ; i++)
    {
        GBRef val = GBArrayGetValueAtIndex(array, i);
        GBRelease( val );
    }
}

void GBArrayIterate(const GBArray* array , GBContainerIterator method , void* context)
{
    if( array && method)
    {
        for (GBIndex i = 0; i <  GBArrayGetSize(array) ; i++)
        {
            GBRef val = GBArrayGetValueAtIndex(array, i);
            if( method(array , val , context) == 0)
                return;
        }
    }
}
