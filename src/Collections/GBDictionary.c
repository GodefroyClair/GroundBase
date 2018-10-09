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
//  GBDictionary.c
//  GroundBase
//
//  Created by Manuel Deneu on 28/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//



#include <string.h>
#include <GBDictionary.h>
#include "../Private/Dictionary.h"
#include "GBSequence_Private.h"
#include <GBArray.h>
#include <GBList.h>


#include "../GBObject_Private.h"


static void * Dictionary_ctor(void * _self, va_list * app);
static void * Dictionary_dtor (void * _self);
static void * Dictionary_clone (const void * _self);
static uint8_t  Dictionary_equals (const void * _self, const void * _b);
static GBRef Dictionary_description (const void * self);

static void retainCallback(GBRef _self);
static void releaseCallback(GBRef _self);

//static void dictionaryIteratorSerialize(GBRef dict, const GBString* key , GBRef value , void* context);

//static void dictionaryIteratorClone(GBRef dict, const GBString* key , GBRef value , void* context);

//static void dictionaryIteratorGBIterate(const Dictionary* dict, const char* key , void* value, void* context);
static int dictionaryIteratorComparison(const Dictionary* dict, const char* key , void* value , void* context);
static int dictionaryIteratorRelease(const Dictionary* dict, const char* key , void* value, void* context);

struct _GBDictionary
{
    GBSequenceBase base;
    Dictionary *_dict;
};

static const GBObjectClass _DictionaryClass =
{
    sizeof(struct _GBDictionary),
    Dictionary_ctor,
    Dictionary_dtor,
    Dictionary_clone,
    Dictionary_equals,
    Dictionary_description,
    NULL, // initialize
    NULL, // deinit
    retainCallback,
    releaseCallback,
    (char*)GBDictionaryClassName
};


static GBSequenceCallbacks _GBDictionaryCallbacks =
{
    (_SequenceGetSize)              GBDictionaryGetSize,
    (_SequenceAddValueForKey)       GBDictionaryAddValueForKey,
    (_SequenceContainsKey)          GBDictionaryContains,
    (_SequenceGetValue)             GBDictionaryGetValueForKey,
    (_SequenceIterate)              GBDictionaryIterateValues

};

GBObjectClassRef GBDictionaryClass = & _DictionaryClass;


static void * Dictionary_ctor(void * _self, va_list * app)
{
    UNUSED_PARAMETER( app );
    
    GBDictionary *self = _self;
    
    if( self)
    {
    
        if( GBSequenceBaseInit(&self->base, _GBDictionaryCallbacks))
        {
            self->_dict = DictionaryInit();
        
            return self;
        }
        
    }
    return NULL;
}
static void * Dictionary_dtor (void * _self)
{
    GBDictionary* self = (GBDictionary*) _self;
    
    DictionaryFree(self->_dict);
    
    return self;
}

static void retainCallback( GBRef _self)
{
    UNUSED_PARAMETER(_self);
}
static void releaseCallback( GBRef _self)
{
    if( GBObjectGetRefCount( _self) == 0)
    {
        GBDictionary* self =(GBDictionary* ) _self;
        GBDictionaryClear( self);

    }
}



static int dictionaryIteratorClone(GBRef dict, const GBString* key , GBRef value , void* context)
{
    UNUSED_PARAMETER(dict);
    GBDictionary* out = context;
    
    GBDictionaryAddValueForKey(out, value, key);
    
    return 1;
}

static void * Dictionary_clone (const void * _self)
{
    
    const GBDictionary* self = _self;
    
    if( self)
    {
        GBDictionary* dict = GBDictionaryInit();
        GBDictionaryIterateValues(self, dictionaryIteratorClone, dict);
        
        return dict;
    }
    return NULL;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

struct DictionaryComparator
{
    const Dictionary* d2;
    size_t accum; // must match DictionaryGetSize after iteration
};

static int dictionaryIteratorComparison( const Dictionary* dict, const char* key , void* value , void* context)
{
    UNUSED_PARAMETER(dict);
    struct DictionaryComparator *comp = context;
    const Dictionary *d2 = comp->d2;

    
    
    if( DictionaryContainsKey(d2, key))
    {
        const void* v2 = DictionaryGetValueForKey(d2, key);
        
        if( GBObjectEquals(value, v2) )
        {
            comp->accum++;
            
            return 1; // continue
        }
    }
    
    return 0; // stop the iteration
}



static uint8_t  Dictionary_equals (const void * _self, const void * _b)
{
    const GBDictionary* d1 = _self;
    const GBDictionary* d2 = _b;
    
    if( GBDictionaryGetSize( d1 ) != GBDictionaryGetSize(d2))
        return 0;
    
    struct DictionaryComparator comp = { d2->_dict , 0};
    
    
    DictionaryIterateValues(d1->_dict, dictionaryIteratorComparison , &comp);
    
    return comp.accum == GBDictionaryGetSize( d1 );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

static GBRef Dictionary_description (const void * _self)
{
    const GBDictionary *self =(const GBDictionary *) _self;
    if( self)
    {
        return GBStringInitWithFormat("%zi keys" , GBDictionaryGetSize(self));
    }
    
    return NULL;
}

GBDictionary* GBDictionaryInit()
{
    return GBObjectAlloc( GBDefaultAllocator,GBDictionaryClass);
}




BOOLEAN_RETURN uint8_t GBDictionaryAddValueForKey(GBDictionary* dict, GBRef value ,  const GBString* key)
{

    if (value == NULL)
        return 0;
    
    const uint8_t ret = DictionaryAddValueForKey(dict->_dict,(char*) GBStringGetCStr(key),(void*) value);
    
    if( ret)
        GBRetain(value);
    
    return ret;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

static int dictionaryIteratorRelease(const Dictionary* dict, const char* key , void* value, void* context)
{
    UNUSED_PARAMETER(key);
    UNUSED_PARAMETER(context);
    UNUSED_PARAMETER(dict);
    
    if( GBObjectIsValid(value))
    {
        GBRelease(value);
    }
    
    return 1;
}

BOOLEAN_RETURN uint8_t GBDictionaryClear( GBDictionary* dict )
{
    DictionaryIterateValues(dict->_dict, dictionaryIteratorRelease , NULL);
    return DictionaryClear( dict->_dict );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

BOOLEAN_RETURN uint8_t GBDictionaryContains(const GBDictionary* dict , const GBString* key)
{
    return DictionaryContainsKey(dict->_dict, GBStringGetCStr(key));
}

BOOLEAN_RETURN uint8_t GBDictionaryRemove( GBDictionary* dict , const GBString* key)
{
    GBRef obj = DictionaryGetValueForKey(dict->_dict, GBStringGetCStr(key));
    
    if( obj)
    {
        GBRelease(obj);
    }
    
    return DictionaryRemoveKey(dict->_dict, GBStringGetCStr(key));
}
GBRef GBDictionaryGetValueForKey(const GBDictionary* dict, const GBString *key)
{    
    return DictionaryGetValueForKey(dict->_dict, GBStringGetCStr(key));
}

GBSize GBDictionaryGetSize(const GBDictionary *dict)
{
    return DictionaryGetSize(dict->_dict);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

struct GetKeysContext
{
    GBContainer* collection;
    GBIndex accum;
};
static int Internal_GBDictGetKeys( const Dictionary* dict, const char* key , void* value , void* context)
{
    DEBUG_ASSERT(dict);
    UNUSED_PARAMETER(dict);
    
    UNUSED_PARAMETER(value);
    struct GetKeysContext* ctx = (struct GetKeysContext*) context;
    DEBUG_ASSERT(ctx);
    
    if( ctx)
    {
        const GBString* k = GBStringInitWithCStr(key);
        GBArrayAddValue( ctx->collection , k);
        GBRelease(k);
    }
    
    return 1;
}
GBArray* GBDictionaryGetKeyList( const GBDictionary* dict)
{
    if( dict)
    {
        struct GetKeysContext ctx;
        ctx.accum = 0;
        ctx.collection = GBArrayInit();
        
        DictionaryIterateValues(dict->_dict, Internal_GBDictGetKeys, &ctx);
        
        return ctx.collection;
    }
    return NULL;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

struct IterateKeysContext
{
    const GBDictionary* dict;
    GBSequenceIterator method;
    void* methodContext;
};
static int Internal_IterateKeys(const GBContainer* container, GBRef value , void* ctx)
{
    UNUSED_PARAMETER(container);
    struct IterateKeysContext* context = (struct IterateKeysContext*) ctx;

    DEBUG_ASSERT(context);

    return context->method(context->dict , value , GBDictionaryGetValueForKey(context->dict, value) , context->methodContext);
    
}

void GBDictionaryIterateValues( const GBDictionary* dict , GBSequenceIterator method , void*context)
{
    if( dict && method)
    {
        const GBContainer* keys = GBDictionaryGetKeyList(dict);
        
        struct IterateKeysContext ctx;
        ctx.dict = dict;
        ctx.method = method;
        ctx.methodContext = context;
        GBContainerIterate(keys, Internal_IterateKeys, &ctx);
        
        GBRelease(keys);
    }
}

