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
//  GBJSON.c
//  GroundBase
//
//  Created by Manuel Deneu on 23/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include <string.h>
#include <json.h>
#include <GBNumber.h>
#include <GBDictionary.h>
#include <GBArray.h>
#include <GBList.h>
#include <GBContainer.h>
#include "GBJSON.h"



static GBObject* JSON_GetNum( const struct json_object* object);
static GBObject* JSON_GetDictionary( const struct json_object* object);
static GBObject* JSON_GetArray( const struct json_object* object);
static GBObject* JSON_GetString( const struct json_object* object);

static GBObject* JSON_GetGBObject( const struct json_object* object);

static BOOLEAN_RETURN uint8_t GBRefGetJSON( GBRef value , struct json_object** object);

static GBObject* JSON_GetGBObject( const struct json_object* object)
{
    
    GBObject* ret = NULL;
    switch (json_object_get_type(object))
    {
            
        case json_type_boolean:
        case json_type_double:
        case json_type_int:
            ret = JSON_GetNum(object);
            break;
            
        case json_type_object:
            ret = JSON_GetDictionary(object);
            break;
        case json_type_array:
            ret = JSON_GetArray(object);
            break;
        case json_type_string:
            ret = JSON_GetString(object);
            break;
        case json_type_null:
        default:
            DEBUG_ASSERT(0); // should not occur
            break;
    }
    
    if( ret)
    {
        
    }
    
    return ret;
}

GBObject* GBJSONParse( const char* buffer , GBSize size)
{
    
    if( buffer && size > 0 && size != GBSizeInvalid)
    {
        
        enum json_tokener_error err = json_tokener_error_depth;
        struct json_object* root = NULL;
        
        root = json_tokener_parse_verbose( buffer , &err );

        if( err == json_tokener_success && root)
        {
            GBObject* ret = JSON_GetGBObject(root);
            
            json_object_put(root);
            return ret;
        }
        
        return NULL;
    }
    return NULL;
}

static GBObject* JSON_GetNum( const struct json_object* object)
{
    DEBUG_ASSERT(object);
    
    switch (json_object_get_type(object))
    {
        case json_type_boolean:
            return GBNumberInitWithInt((int) json_object_get_boolean(object));
        case json_type_double:
            return GBNumberInitWithDouble( json_object_get_double(object));
        case json_type_int:
            return GBNumberInitWithInt( json_object_get_int(object));
        default:
            DEBUG_ASSERT(0);
            break;
    }
    
    
    return NULL;
}

static GBObject* JSON_GetDictionary( const struct json_object* object)
{
    DEBUG_ASSERT(object);
    
    GBDictionary* ret = GBDictionaryInit();
    
    json_object_object_foreach( object , itemKey, itemJson)
    {
        GBRef val = JSON_GetGBObject(itemJson);
        const GBString* key = GBStringInitWithCStr(itemKey);
        GBDictionaryAddValueForKey(ret, val, key);
        GBRelease(key);
        GBRelease(val);
        
    }
    
    
    return ret;
}
static GBObject* JSON_GetArray( const struct json_object* object)
{
    DEBUG_ASSERT(object);
    
    const GBSize jsonSize = json_object_array_length(object);
    
    GBArray* ret = GBArrayInit();
    
    for (GBIndex i = 0 ; i < jsonSize ; i++)
    {
        const struct json_object* itemJson = json_object_array_get_idx(object, i);
        
        GBRef val = JSON_GetGBObject(itemJson);
        
        if( val)
        {
            GBArrayAddValue(ret, val);
            GBRelease(val);
        }
        else
        {
            DEBUG_ASSERT(0);
        }
        
    }
    
    return ret;
    
}
static GBObject* JSON_GetString( const struct json_object* object)
{
    DEBUG_ASSERT(object);
    
    
    return GBStringInitWithCStr( json_object_get_string( CONST_CAST(struct json_object*) object));
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
static json_object* JSONCreateNumber( GBRef number)
{
    json_object* numberObj = json_object_new_object();
    
    if( numberObj == NULL)
        return 0;
    
    switch (GBNumberGetType(number))
    {
        case GBNumberTypeInt:
            json_object_object_add(numberObj, TYPE_ATTRIBUTE_NAME, json_object_new_string(GBNumberTypeName_Int));
            json_object_object_add(numberObj, VALUE_NAME_TOKEN,  json_object_new_int(GBNumberGetInt(number) ));
            break;
            
        case GBNumberTypeFloat:
            json_object_object_add(numberObj, TYPE_ATTRIBUTE_NAME, json_object_new_string(GBNumberTypeName_Float));
            json_object_object_add(numberObj, VALUE_NAME_TOKEN,  json_object_new_double((float)GBNumberGetFloat(number) ));
            break;
            
        case GBNumberTypeDouble:
            json_object_object_add(numberObj, TYPE_ATTRIBUTE_NAME, json_object_new_string(GBNumberTypeName_Double));
            json_object_object_add(numberObj, VALUE_NAME_TOKEN,  json_object_new_double( GBNumberGetDouble(number) ));
            break;
            
        case GBNumberTypeLong:
            json_object_object_add(numberObj, TYPE_ATTRIBUTE_NAME, json_object_new_string(GBNumberTypeName_Long));
            json_object_object_add(numberObj, VALUE_NAME_TOKEN,  json_object_new_int64( GBNumberGetLong(number) ));
            break;
            
        default:
            break;
    }
    
    return numberObj;
}
 */
static json_object* JSONCreateNumber( GBRef number)
{
    /*
    json_object* numberObj = json_object_new_object();
    
    if( numberObj == NULL)
        return 0;
    */
    switch (GBNumberGetType(number))
    {
        case GBNumberTypeInt:
            return json_object_new_int(GBNumberGetInt(number) );
            /*
            json_object_object_add(numberObj, TYPE_ATTRIBUTE_NAME, json_object_new_string(GBNumberTypeName_Int));
            json_object_object_add(numberObj, VALUE_NAME_TOKEN,  json_object_new_int(GBNumberGetInt(number) ));
            break;
            */
        case GBNumberTypeFloat:
            return json_object_new_double((float)GBNumberGetFloat(number) );
            /*
            json_object_object_add(numberObj, TYPE_ATTRIBUTE_NAME, json_object_new_string(GBNumberTypeName_Float));
            json_object_object_add(numberObj, VALUE_NAME_TOKEN,  json_object_new_double((float)GBNumberGetFloat(number) ));
            break;
            */
        case GBNumberTypeDouble:
            return json_object_new_double( GBNumberGetDouble(number) );
            /*
            json_object_object_add(numberObj, TYPE_ATTRIBUTE_NAME, json_object_new_string(GBNumberTypeName_Double));
            json_object_object_add(numberObj, VALUE_NAME_TOKEN,  json_object_new_double( GBNumberGetDouble(number) ));
            break;
             */
            
        case GBNumberTypeLong:
            return json_object_new_int64( GBNumberGetLong(number) );
            /*
            json_object_object_add(numberObj, TYPE_ATTRIBUTE_NAME, json_object_new_string(GBNumberTypeName_Long));
            json_object_object_add(numberObj, VALUE_NAME_TOKEN,  json_object_new_int64( GBNumberGetLong(number) ));
            break;
            */
        default:
            DEBUG_ASSERT(0);
            break;
    }
    
    return NULL;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


static int Iterate_Container(const GBContainer* container, GBRef value , void* context)
{
    UNUSED_PARAMETER(container);
    DEBUG_ASSERT(context);
    struct json_object* object = context;
    
    struct json_object* jsonVal = NULL;
    if(GBRefGetJSON(value, &jsonVal))
    {
        if(json_object_array_add( object, jsonVal) == 0)
        {
            
            
        }
        else
        {
            DEBUG_LOG("[GBJSON] Unable to add object : ");
            GBObjectLog(value);
        }
    }
    return 1;
}


struct DictIteratorContext
{
    struct json_object* object;
    const GBSequence* sequence;
} ;

static int Iterate_Dictionary(const GBContainer* container, GBRef value , void* ctx)
{
    UNUSED_PARAMETER(container);
    
    DEBUG_ASSERT(ctx);
    DEBUG_ASSERT(value);
    
    struct DictIteratorContext* context = ctx;
    
    DEBUG_ASSERT(context->object && context->sequence);
    
    
    const GBString* itemKey =  value;
    
    GBRef itemVal =  GBSequenceGetValueForKey(context->sequence, itemKey);
    
    struct json_object* jsonVal =NULL;
    if(GBRefGetJSON(itemVal , &jsonVal))
    {
        json_object_object_add(context->object, GBStringGetCStr(itemKey), jsonVal);
    }
    
    
    return 1;
    
}



static BOOLEAN_RETURN uint8_t GBRefGetJSON( GBRef value , struct json_object** object)
{
    DEBUG_ASSERT(object);
    
    
    if( !value)
    {
        *object = NULL;
        return 1;
    }
    else if( IsKindOfClass(value, GBStringClass))
    {
        *object =  json_object_new_string( GBStringGetCStr(value));
        
        return *object != NULL;
    }
    else if( IsKindOfClass(value, GBNumberClass))
    {
        *object =  JSONCreateNumber(value);
        return *object != NULL;
    }
    else if( isGBContainter(value))
    {
        
        *object  = json_object_new_array();
        
        GBContainerIterate(value, Iterate_Container, *object);
        
        return 1;
    }
    else if (IsKindOfClass(value, GBDictionaryClass) )
    {
        
        const GBRef listKeys = GBDictionaryGetKeyList(value);
        //const GBSize numKeys = GBContainerGetSize(listKeys);
        
        *object  = json_object_new_object();
        
        
        struct DictIteratorContext _ctx;
        _ctx.object = *object;
        _ctx.sequence = value;
        GBContainerIterate(listKeys, Iterate_Dictionary, &_ctx);
        
        /*
        
        GBSize addAccum = 0;
        for (GBIndex i = 0; i<numKeys ;i++)
        {
            const GBString* itemKey =  GetValueAtIndex(listKeys, i);
            GBRef itemVal =  GBSequenceGetValueForKey(value, itemKey);
            
            struct json_object* jsonVal =NULL;
            if(GBRefGetJSON(itemVal , &jsonVal))
            {
                addAccum+=json_object_object_add(*object, GBStringGetCStr(itemKey), jsonVal) == 0;
            }
            
        }
        DEBUG_ASSERT(numKeys == addAccum);
         */
        GBRelease(listKeys);
        
        return 1;
    }
    
    else
    {
        
        GBRef className =  GBObjectGetClassName(value);
        DEBUG_LOG("[GBRefGetJSON] Unimplemented converter for Class %s\n" , GBStringGetCStr( className ));
        GBRelease(className);
        DEBUG_ASSERT(0);
        *object = NULL;
        
        
    }
    DEBUG_ASSERT(0);
    return 1;
    
}

char* GBJSONWriteBuffer( GBRef objectToWrite )
{
    if( objectToWrite )
    {
        struct json_object *object = NULL;
        
        if(GBRefGetJSON(objectToWrite, &object))
        {
            const char* str =  json_object_to_json_string(object);
            
            char*ret = strdup(str);
            json_object_put(object);
            
            return ret;
            
        }
    }
    return NULL;
}

