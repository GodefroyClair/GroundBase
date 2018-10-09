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
//  GBList.c
//  GroundBase
//
//  Created by Manuel Deneu on 28/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//
/*
 
 */

#include <string.h>

#include <GBCommons.h>
#include <GBList.h>

#include "../GBObject_Private.h"
#include "GBContainer_Private.h"
#include "../GBAllocator.h"

static void * GBList_ctor(void * _self, va_list * app);
static void * GBList_dtor (void * _self);
static void * GBList_clone (const void * _self);
static uint8_t  GBList_equals (const void * _self, const void * _b);
static GBRef GBList_description (const void * self);





static void retainCallback( GBRef _self);
static void releaseCallback( GBRef _self);

struct _GBList
{
    GBContainerBase super;
    List* _list;
};


static  GBContainerCallbacks listCallbacks =
{
    (_GBContainerGetSize)         GBListGetSize,
    (_GBContainerAddValue)        GBListAddValue,
    (_GBContainerRemoveValue)     GBListRemoveValue,
    (_GBContainerContainsValue)   GBListContainsValue,
    (_GBContainerIterate)         GBListIterate
    
} ;

static const GBObjectClass _SetClass =
{
    sizeof(struct _GBList),
    GBList_ctor,
    GBList_dtor,
    GBList_clone,
    GBList_equals,
    GBList_description,
    NULL, // initialize
    NULL, // deinit
    //serialize,
    retainCallback,
    releaseCallback,
    (char*)GBListClassName
};


GBObjectClassRef GBListClass = & _SetClass;

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

static void * GBList_ctor(void * _self, va_list * app)
{
    UNUSED_PARAMETER(app);
    GBList *self = _self;
    
    if( self)
    {
        if( GBContainerBaseInit(&self->super, listCallbacks ))
        {
            self->_list = ListInit();
            return self;
        }
    }
    
    
    
    return NULL;
}

static void * GBList_dtor (void * _self)
{
    GBList *self = _self;
    
    GBListReleaseContent(self);
    ListFree(self->_list);
    
    
    return _self;
}

static void retainCallback( GBRef _self)
{
    UNUSED_PARAMETER(_self);
}
static void releaseCallback( GBRef _self)
{
    UNUSED_PARAMETER(_self);
}

static void * GBList_clone (const void * _self)
{
    const GBList* self = _self;
    
    if (self)
    {
        GBList* dest = GBListInit();
        
        if( dest == NULL)
            return NULL;
        
        GBRef value = NULL;
        GBListForEach(self, value)
        {
            GBListAddValue(dest, value);
        }
        /*
        for (GBIndex i = 0; i< GBListGetSize(self) ; i++)
        {
            GBListAddValue(dest, GBListGetValueAtIndex(self, i));
        }
         */
        return dest;
    }
    return NULL;
}

static uint8_t GBList_equals (const void * _self, const void * _b)
{
    const GBList* self = _self;
    const GBList* other = _b;
    
    if( GBListGetSize(self) != GBListGetSize(other))
        return 0;
    
    
    GBRef selfVal = NULL;
    
    GBListForEach(self, selfVal)
    {
        if( GBListContainsValue( other, selfVal) == 0)
        {
            return 0;
        }
    }
    

    return 1;
}


static GBRef GBList_description (const void * _self)
{
    const GBList* self = (const GBList* ) _self;
    if( self)
    {
        return GBStringInitWithFormat("GBList %zi" , GBListGetSize(self));
    }
    return NULL;
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


GBList* GBListInit()
{
    return GBObjectAlloc( GBDefaultAllocator,GBListClass);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 Methods conforming to GBCollectionType
 */

size_t GBListGetSize(const GBList* list)
{

    if( list == NULL )
        return 0;

    return ListGetSize( list->_list);
}


BOOLEAN_RETURN uint8_t GBListClear( GBList* list)
{
    GBListReleaseContent(list);
    return ListRemoveAll( list->_list );
}

BOOLEAN_RETURN uint8_t GBListAddValue(GBList* list , GBRef value)
{
    const uint8_t ret = ListAddValue( list->_list, CONST_CAST(void*) value);
    
    if( ret)
    {
        GBRetain(value);
    }
    return ret;

}

BOOLEAN_RETURN uint8_t GBListRemoveValueAtIndex( GBList* list , GBIndex index)
{
    GBRef value = GBListGetValueAtIndex( list, index);
    if( value)
    {
        GBRelease(value);
    }
    return ListRemoveValueAtIndex( list->_list, index);
}

BOOLEAN_RETURN uint8_t GBListRemoveValue( GBList* list , GBRef value)
{
    void* ptrToRemove = NULL;
    void* ptr = NULL;
    ListForEach( list->_list , ptr)
    {
        GBRef object = ptr;
        DEBUG_ASSERT(GBObjectIsValid(object));
        if( GBObjectEquals(object, value))
        {
            ptrToRemove = ptr;
            break;
        }
    }
    
    if( ptrToRemove)
    {
        GBRelease(ptrToRemove);
        
        if( ListRemove(list->_list, ptrToRemove))
        {
            return 1;
        }
    }
    
    
    return 0;
    
}


GBIndex GBListGetIndexForValue( const GBList *list , GBRef value)
{
    for (GBIndex i = 0; i < GBListGetSize( list ) ; i++)
    {
        if( GBObjectEquals( GBListGetValueAtIndex( list , i) , value))
        {
            return i;
        }
    }
    return GBIndexInvalid;
    
}

GBRef GBListGetValueAtIndex( const GBList *list , GBIndex index)
{
    return ListGetValueAtIndex( list->_list, index);
}

BOOLEAN_RETURN uint8_t GBListContainsValue(const GBList* list , GBRef item)
{
    GBRef v = NULL;
    
    GBListForEach(list, v)
    {
        if( GBObjectEquals(v , item))
        {
            return 1;
        }
    }

    return 0;
}

void GBListReleaseContent(const GBList* list)
{
    GBRef v= NULL;
    GBListForEach(list, v)
    {
        GBRelease(v);
    }
}

void GBListIterate(const GBList* list , GBContainerIterator method , void* context)
{
    if( list && method)
    {
        GBRef v= NULL;
        GBListForEach(list, v)
        {
            if(method(list , v , context) == 0)
                return;
        }
    }
}


GBListIterator* GBListBegin( const GBList* list)
{
    return ListBegin( list->_list );
}
GBListIterator* GBListGetNext( const GBListIterator* iter)
{
    return ListGetNext(iter);
}
GBRef GBListGetValue( const GBListIterator* iter)
{
    return ListGetValue( iter );
}
