/*
 * Copyright (c) 2016-2017 FlyLab
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
//  GBBinCoder.c
//  GroundBase
//
//  Created by Manuel Deneu on 19/09/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//
#include <string.h>
#include <GBBinCoder.h>
#include <GBContainer.h>
#include <GBDictionary.h>
#include <GBList.h>
#include "../GBObject_Private.h"
#include "Binn/binn.h"

static binn* ConvertGBObjectToBinn(GBRef object);
static binn* ConvertGBStringToBinn( const GBString* object);
static binn* ConvertGBNumberToBinn( const GBNumber* object);
static binn* ConvertGBSequenceToBinn( const GBSequence* object);
static binn* ConvertGBContainerToBinn( const GBContainer* object);

static GBObject* ConvertBinnToGBObject(binn* object);

static GBNumber* BinnGetNumber(binn* val);
static GBSequence* BinnGetSequence(binn* object);
static GBContainer* BinnGetContainer(binn*object , GBObjectClassRef class);

static void * GBBinCtor(void * _self, va_list * app);
static void * GBBinDtor (void * _self);
static uint8_t  GBBinEquals (const void * _self, const void * _b);
static GBRef GBBinGetDescription (const void * self);

struct _GBBinCoder
{
    GBObjectBase base;
    binn* _ptr;
};



static const GBObjectClass _GBBinCoderClass =
{
    sizeof(struct _GBBinCoder),
    GBBinCtor,
    GBBinDtor,
    NULL,
    GBBinEquals,
    GBBinGetDescription,
    NULL, // init
    NULL, // deinit
    NULL, //retain
    NULL, // release
    (char*)GBBinCoderClassName
};

GBObjectClassRef GBBinCoderClass = & _GBBinCoderClass;

static void * GBBinCtor(void * _self, va_list * app)
{
    UNUSED_PARAMETER(app);
    GBBinCoder* self = _self;

    return self;
}
static void * GBBinDtor (void * _self)
{
    GBBinCoder* self = _self;
    
    if( self)
    {
        if( self->_ptr)
        {
            binn_free(self->_ptr);
        }
        
        return self;
    }
    
    return NULL;
}

static uint8_t  GBBinEquals (const void * _self, const void * _b)
{
    UNUSED_PARAMETER(_self);
    UNUSED_PARAMETER(_b);
    DEBUG_ASSERT(0);
    return 0;
}
static GBRef GBBinGetDescription (const void * self)
{
    UNUSED_PARAMETER(self);
    return NULL;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GBBinCoder* GBBinCoderInit()
{
    GBBinCoder* self = GBObjectAlloc(GBDefaultAllocator, GBBinCoderClass);
    
    if( self)
    {
        self->_ptr = binn_object();
        return self;
    }
    
    return NULL;
    
}

GBBinCoder* GBBinCoderInitWithContent(const void* buf )
{
    if( buf )
    {
        binn * ptr =binn_open( buf );
        
        if( ptr)
        {
            GBBinCoder* self = GBObjectAlloc(GBDefaultAllocator, GBBinCoderClass);
            
            if( self)
            {
                self->_ptr = ptr;
                
                return self;
            }
            else
            {
                binn_free(ptr);
            }
        }
    }
    
    return NULL;
}

GBBinCoder* GBBinCoderInitWithRootObject( GBRef object)
{
    if( object && GBObjectIsValid(object))
    {
        
        if( !IsKindOfClass(object, GBDictionaryClass))
        {
            DEBUG_LOG("[GBBinCoderInitWithRootObject] works only with GBDictionary for now \n ");
            return NULL;
        }
        binn* p =ConvertGBObjectToBinn(object);
        
        if( !p)
            return NULL;
        
        
        GBBinCoder* self = GBObjectAlloc(GBDefaultAllocator, GBBinCoderClass);
        
        if( self)
        {
            self->_ptr = p;
            return self;
        }
        
        
    }
    
    return NULL;
}

GBObject* GBBinCoderDecodeRoot( const GBBinCoder* decoder)
{
    if( decoder && decoder->_ptr)
    {
        return ConvertBinnToGBObject(decoder->_ptr);
    }
    return NULL;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GBObject* GBBinCoderDecodeRootWithType( const GBBinCoder* decoder, GBObjectClassRef classType)
{
    if( classType && decoder && decoder->_ptr)
    {
        GBObject* r = ConvertBinnToGBObject(decoder->_ptr);
        
        if (r == NULL)
            return NULL;
        
        if( IsKindOfClass(r, classType))
        {
            return r;
        }
        else
        {
            GBRelease(r);
            return NULL;
        }
    }
    
    return NULL;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GBSize GBBinCoderGetBufferSize( const GBBinCoder* coder)
{
    return (GBSize) binn_size(coder->_ptr);
}


const uint8_t* GBBinCoderGetBuffer( const GBBinCoder* coder)
{
    if( coder)
    {
        return binn_ptr(coder->_ptr);
    }
    return NULL;
}

BOOLEAN_RETURN uint8_t GBBinCoderWriteBuffer( const GBBinCoder* coder , char*bufferOut)
{
    if( coder && bufferOut)
    {
        return memcpy(bufferOut, binn_ptr( coder->_ptr), GBBinCoderGetBufferSize( coder)) != NULL;
    }
    return 0;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
     Encode
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

static binn* ConvertGBObjectToBinn(GBRef object)
{
    if(object && GBObjectIsValid(object))
    {
        if( IsKindOfClass(object, GBStringClass))
            return ConvertGBStringToBinn(object);
        
        if( IsKindOfClass(object, GBNumberClass))
            return ConvertGBNumberToBinn(object);
        
        else if( isGBContainter(object))
        {
            return ConvertGBContainerToBinn(object);
        }
        else if( isGBSequence(object))
            return ConvertGBSequenceToBinn(object);
        
        DEBUG_LOG("[ConvertGBObjectToBinn] Will assert for object '%s'\n" , GBStringGetCStr( GBObjectGetClassName(object) ));
        GBObjectLog(object);
        DEBUG_ASSERT(0);
        
        
    }
    return NULL;
}

static binn* ConvertGBStringToBinn( const GBString* object)
{
    if(object)
    {
        return binn_string( (char*) GBStringGetCStr(object), NULL);
    }
    return NULL;
}

static binn* ConvertGBNumberToBinn( const GBNumber* object)
{
    if(object)
    {
        switch (GBNumberGetType(object))
        {
                
            case GBNumberTypeInt:
                return binn_int32( GBNumberGetInt(object));

            case GBNumberTypeDouble:
                return binn_double( GBNumberGetDouble(object));
                
            case GBNumberTypeFloat:
                return binn_float(GBNumberGetFloat(object));
                
            case GBNumberTypeLong:
                return binn_int64(GBNumberGetLong(object));
                
                
            case GBNumberTypeUnknown:
            default:
                DEBUG_ASSERT(0);
                break;
        }
    }
    return NULL;
}
/* **** **** **** **** **** **** **** **** */

struct SeqIteratorContext
{
    binn* objectRoot;
};

static int SequenceIterator(const GBSequence* sequence, const GBString* key , GBRef value , void* context)
{
    UNUSED_PARAMETER(sequence);
    struct SeqIteratorContext* ctx = context;
    DEBUG_ASSERT(ctx);
    DEBUG_ASSERT(ctx->objectRoot);
    binn* v = ConvertGBObjectToBinn(value);
    if( v)
    {
        binn_object_set_value(ctx->objectRoot, (char*) GBStringGetCStr(key), v);
        binn_free(v);
    }
    
    return 1;
}


static binn* ConvertGBSequenceToBinn( const GBSequence* object)
{
    
    if( object)
    {
        struct SeqIteratorContext ctx;
        ctx.objectRoot = binn_object();
        
        GBSequenceIterateValues(object, SequenceIterator, &ctx);
        
        return ctx.objectRoot;
    }
    return NULL;
}

/* **** **** **** **** **** **** **** **** */

static int ContainerIterator(const GBContainer* container, GBRef value , void* context)
{
    UNUSED_PARAMETER(container);
    binn* list = context;
    DEBUG_ASSERT(list);
    
    binn* v = ConvertGBObjectToBinn(value);
    if( v)
    {
        binn_list_add_new(list, v);
        
        
    }
    
    return 1;
}

static binn* ConvertGBContainerToBinn( const GBContainer* object)
{
    if( object)
    {
        binn* container = binn_list();
        GBContainerIterate(object, ContainerIterator, container);
        
        return container;
    }
    return NULL;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

BOOLEAN_RETURN uint8_t GBBinCoderEncodeString( GBBinCoder* coder, const GBString* value ,const GBString* key )
{
    if( coder && value && key && !GBStringIsEmpty(key))
    {

        DEBUG_ASSERT(coder->_ptr);
        
        
        binn* val = ConvertGBStringToBinn(value);
        
        const uint8_t ret = binn_object_set_value(coder->_ptr, (char*)GBStringGetCStr(key), val);
        binn_free(val);
        return ret;
        //return binn_object_set_str( coder->_ptr , (char*)GBStringGetCStr(key), (char*) GBStringGetCStr(value));
        
    }
    return 0;
}

BOOLEAN_RETURN uint8_t GBBinCoderEncodeNumber( GBBinCoder* coder, const GBNumber* value ,const GBString* key )
{
    if( coder && value && key && !GBStringIsEmpty(key))
    {
        binn* val = ConvertGBNumberToBinn(value);
        
        const uint8_t ret = binn_object_set_value( coder->_ptr, (char*) GBStringGetCStr(key), val) == 1;
        binn_free(val);
        
        return ret;
            
    }
    return 0;
}

BOOLEAN_RETURN uint8_t GBBinCoderEncodeSequence( GBBinCoder* coder, const GBSequence* value ,const GBString* key )
{
    if( coder && value && key && !GBStringIsEmpty(key))
    {
        binn* val = ConvertGBSequenceToBinn(value);
        
        const uint8_t ret = binn_object_set_value( coder->_ptr, (char*) GBStringGetCStr(key), val) == 1;
        binn_free(val);
        
        return ret;
        
    }
    
    return 0;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
     Decode
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

static GBObject* ConvertBinnToGBObject(binn* object)
{
    if( object)
    {
        GBObject* out = NULL;
        
        out = BinnGetNumber(object);
        if( out)
        {
            return out;
        }
        
        switch (binn_type( object))
        {
            case BINN_STRING:
                return GBStringInitWithCStr(binn_get_str(object));
                
            case BINN_MAP:
            case BINN_OBJECT:
                return BinnGetSequence(object);
            case BINN_LIST:
                return BinnGetContainer(object , GBArrayClass);
            default:
                DEBUG_LOG("[ConvertBinnToGBObject] Other type %x \n" , binn_type( object));
                DEBUG_ASSERT(0);
                break;
        }
    }
    
    
    return NULL;
}
GBString* GBBinCoderDecodeString( const GBBinCoder* coder ,const GBString* key )
{
    if (coder && key)
    {
        char* val;
        binn_object_get_str(coder->_ptr, (char*) GBStringGetCStr(key), &val);
        
        if (val)
        {
            return GBStringInitWithCStr(val);
        }
    }
    return NULL;
}


static GBNumber* BinnGetNumber(binn* val)
{
    if( val)
    {
        switch (binn_type( val))
        {
            case BINN_UINT8:
            case BINN_INT8:
            case BINN_UINT16:
            case BINN_INT16:
            case BINN_UINT32:
            case BINN_INT32:
            {
                int v= 0;
                if( binn_get_int32( val, &v))
                {
                    return GBNumberInitWithInt(v);
                }
            }
                break;
            case BINN_FLOAT:
            {
                double v = 0.0f;
                
                if( binn_get_double( val , &v))
                {
                    return GBNumberInitWithFloat((float)v);
                }
            }
                break;
                
            case BINN_DOUBLE:
            {
                double v = 0.0f;
                
                if( binn_get_double( val , &v))
                {
                    return GBNumberInitWithDouble( v);
                }
            }
                break;
                
            case BINN_INT64:
            {
                int64 v = 0.0f;
                
                if( binn_get_int64( val , &v))
                {
                    return GBNumberInitWithLong( v);
                }
            }
                break;
            default:
                
                break;
        };
    }
    
    return NULL;
}


GBNumber* GBBinCoderDecodeNumber( const GBBinCoder* coder ,const GBString* key )
{
    if (coder && key)
    {
        binn val;
        if( binn_object_get_value(coder->_ptr, GBStringGetCStr(key), &val))
        {
            
            return BinnGetNumber(&val);
        }
        
        
        
    }
    return NULL;
}

static GBContainer* BinnGetContainer(binn*list , GBObjectClassRef class)
{
    GBContainer* outList = GBContainerInitWithType(class);
    if( outList == NULL)
    {
        return NULL;
    }
    
    binn_iter iter;
    //char key[256];  // only for the object
    //int  id;        // only for the map
    binn value;
    binn_list_foreach( list, value)
    {
        GBObject* object = ConvertBinnToGBObject(&value);
        if( object)
        {
            GBContainerAddValue(outList, object);
            GBRelease(object);
        }
    }
    
    return outList;
}

GBContainer* GBBinCoderDecodeContainerWithOuputType( const GBBinCoder* coder ,const GBString* key , GBObjectClassRef class )
{
    if (coder && key)
    {
        binn list;
        if( binn_object_get_value(coder->_ptr, GBStringGetCStr(key), &list))
        {
            if( binn_type(&list) != BINN_LIST)
                return NULL;
            
            
            return BinnGetContainer(&list , class);
            
        }
    }
    return NULL;
}

GBContainer* GBBinCoderDecodeContainer( const GBBinCoder* coder ,const GBString* key )
{
    return GBBinCoderDecodeContainerWithOuputType( coder ,key , GBArrayClass );

}


static GBSequence* BinnGetSequence(binn* object)
{
    GBDictionary* outDict = GBDictionaryInit();
    binn_iter iter;
    char key[256];  // only for the object
    //int  id;        // only for the map
    binn value;
    binn_object_foreach(object, key, value)
    {
        GBObject* outObject = ConvertBinnToGBObject(&value);
        if( outObject)
        {
            const GBString* k = GBStringInitWithCStr(key);
            GBDictionaryAddValueForKey(outDict, outObject , k);
            GBRelease(k);
            GBRelease(outObject);
        }
    }
    
    return outDict;
}

GBSequence* GBBinCoderDecodeSequence( const GBBinCoder* coder ,const GBString* inKey )
{
    if (coder && inKey)
    {
        binn object;
        if( binn_object_get_value(coder->_ptr, GBStringGetCStr(inKey), &object))
        {
            if( binn_type(&object) != BINN_OBJECT)
                return NULL;
            
            if( binn_type(&object) == BINN_MAP)
            {
                DEBUG_ASSERT(0); // needs attention
            }
            
            return BinnGetSequence(&object);
            
        }
        
    }
    return NULL;
}

