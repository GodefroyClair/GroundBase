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
//  GBSet.c
//  GroundBase
//
//  Created by Manuel Deneu on 02/01/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include "GBSet.h"
#include "../GBAllocator.h"
#include "GBContainer_Private.h"

#include "../Private/LibUt/uthash.h"

static BOOLEAN_RETURN uint8_t Internal_GBSetReleaseContent(GBSet* set );


static GBContainerCallbacks _GBSetCallbacks =
{
    (_GBContainerGetSize)        GBSetGetSize,
    (_GBContainerAddValue)       GBSetAddValue,
    (_GBContainerRemoveValue)    GBSetRemoveValue,
    (_GBContainerContainsValue)  GBSetContainsValue,
    (_GBContainerIterate)        GBSetIterate
};

typedef struct
{
    int id;
    GBRef obj;
    UT_hash_handle hh;
    
} GBSetElement;


static GBSetElement* InternalCreateElement( GBRef obj)
{
    GBSetElement *el = GBMalloc( sizeof( GBSetElement ) );
    el->obj = obj;
    
    return el;
}

struct _GBSet
{
    GBContainerBase super;
    GBSetElement* _set;
};

static void * GBSet_ctor(void * _self, va_list * app);
static void * GBSet_dtor (void * _self);
static uint8_t  GBSet_equals (const void * _self, const void * _b);
static GBRef GBSet_description (const void * self);

static const GBObjectClass _GBSetClass =
{
    sizeof(struct _GBSet),
    GBSet_ctor,
    GBSet_dtor,
    NULL,//RunLoop_clone,
    GBSet_equals,
    GBSet_description,
    NULL, // initialize
    NULL, // deinit
    NULL,
    NULL,
    (char*)GBSetClassName
};

GBObjectClassRef GBSetClass = & _GBSetClass;


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

static void * GBSet_ctor(void * _self, va_list * app)
{
    UNUSED_PARAMETER(app);
    
    GBSet* self = _self;
    
    if( self)
    {

        
        if( GBContainerBaseInit(&self->super, _GBSetCallbacks ) )
        {
            self->_set = NULL;
            return self;
        }
        
        DEBUG_ASSERT(0);
        
        return NULL;
    }
    return NULL;
}

static void * GBSet_dtor (void * _self)
{
    GBSet* self = _self;
    
    if( self)
    {
        Internal_GBSetReleaseContent(self);
        
        return self;
    }
    return NULL;
}

static uint8_t  GBSet_equals (const void * _self, const void * _b)
{
    const GBSet* self = _self;
    const GBSet* b    = _b;
    
    if( GBSetGetSize(self) != GBSetGetSize(b))
    {
        return 0;
    }
    
    GBSetElement* valA = NULL;
    GBSetElement* tmp = NULL;
    
    HASH_ITER(hh, self->_set, valA, tmp)
    {
        if( GBSetContainsValue(b, valA) == 0)
            return 0;
    }
    
    return 1;
}

static GBRef GBSet_description (const void * _self)
{
    const GBSet* self = (const GBSet*) _self;
    
    if( self)
    {
        return GBStringInitWithFormat("GBSet %zi Elements " , GBSetGetSize(self));
    }
    return NULL;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GBSet* GBSetInit(void)
{
    return GBObjectAlloc( GBDefaultAllocator,GBSetClass  );
}

static BOOLEAN_RETURN uint8_t Internal_GBSetReleaseContent(GBSet* set )
{
    GBSetElement* el = NULL;
    GBSetElement* tmp = NULL;

    HASH_ITER(hh, set->_set, el, tmp)
    {
        GBRelease(el->obj);
        HASH_DEL( set->_set, el);
        GBFree(el);

        
        
        
    }
    
    /*
     #define HASH_DELETE(hh,head,delptr)                                      
     
     
     
     */
    
    
    return GBSetGetSize(set) == 0;
}

GBSize GBSetGetSize( const GBSet* set)
{
    return HASH_COUNT( set->_set );
}

BOOLEAN_RETURN uint8_t GBSetAddValue( GBSet* set , GBRef value)
{
    if( GBSetContainsValue(set, value))
    {
        return 0;
    }
    
    GBSetElement* el = InternalCreateElement(value);
    
    if( el )
    {
        HASH_ADD_INT( set->_set, id, el );
        
        DEBUG_ASSERT(set->_set);
        GBRetain(value);
        
        return 1;
    }
    return 0;
}

BOOLEAN_RETURN uint8_t GBSetContainsValue(const GBSet* set , GBRef value)
{
    GBSetElement *e = NULL;
    GBSetElement *tmp = NULL;
    
    HASH_ITER(hh, set->_set, e, tmp)
    {
        if( GBObjectEquals(e->obj, value))
        {
            return 1;
        }
        
    }
    
    return 0;
}

BOOLEAN_RETURN uint8_t GBSetRemoveValue( GBSet* set , GBRef value)
{
    GBSetElement *el = NULL;
    GBSetElement *tmp = NULL;
    
    HASH_ITER(hh, set->_set, el, tmp)
    {
        
        if( GBObjectEquals(el->obj, value))
        {
            HASH_DEL( set->_set, el);
            GBRelease(el->obj);
            GBFree(el);
            return 1;
        }
    }

    return 0;
}

BOOLEAN_RETURN uint8_t GBSetClear( GBSet* set)
{
    return Internal_GBSetReleaseContent( set );
}

void GBSetIterate(const GBSet* set , GBContainerIterator method , void* context)
{
    if( set && method)
    {
        
        GBSetElement *el = NULL;
        GBSetElement *tmp = NULL;
        
        HASH_ITER(hh, set->_set, el, tmp)
        {
            if(method(set , el->obj , context))
                return;
        }
    }
}

/*
GBSetIterator* GBSetCreateIterator(const  GBSet* set)
{
    return NULL;
}
BOOLEAN_RETURN uint8_t GBSetIteratorHasNext( GBSetIterator* it)
{
    return 0;
}
GBRef GBSetIteratorGetNext( GBSetIterator* it )
{
    return NULL;
}
void GBSetIteratorFree(GBSetIterator* it)
{
    
}
 */
