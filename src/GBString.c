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
//  GBString.c
//  GroundBase
//
//  Created by Manuel Deneu on 28/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//


#include <stdarg.h> // va_arg
#include <string.h>
#include <stdlib.h> // Malloc
#include <ctype.h> // isprint
#include <stdio.h>
#include <GBContainer.h>
#include <GBString.h>
#include <GBObject.h>
#include <GBArray.h>
#include "GBHash.h"
#include "GBObject_Private.h"
#include "Utilities/Base64.h"
#include "GBAllocator.h"
#include "Private/List.h"
#include "Private/Array.h"
#include "Private/Dictionary.h"
#include "Private/StringImpl.h"
#include "Private/Set.h"

static void * String_ctor(void * _self, va_list * app);
static void * String_dtor (void * _self);
static void * String_clone (const void * _self);
static uint8_t String_equals (const void * _self, const void * _b);
static GBRef String_description (const void * _self);

static uint8_t String_setContent(struct _String *string , const char* content);


struct _String
{
    GBObjectBase base;
    struct StringImpl *_atom;

};

static const GBObjectClass _StringClass =
{
    sizeof(struct _String),
    String_ctor,
    String_dtor,
    String_clone,
    String_equals,
    String_description,
    NULL, // class init
    NULL, // class release
    //serialize,
    NULL, //retain
    NULL, //release
    (char*)GBStringClassName
};
GBObjectClassRef GBStringClass = & _StringClass;

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */



static void* StrStatic_Malloc(GBSize size , const void *self);
static void* StrStatic_Realloc(void *ptr, GBSize size , const void *self);
static void* StrStatic_Calloc( GBSize count, GBSize size , const void *self);
static void  StrStatic_Free( void* ptr , const void *self);

/*
 GBRuntime has special knowledge of this allocator. 
 */
GBAllocator StaticStringAllocator =
{
    StrStatic_Malloc,
    StrStatic_Realloc,
    StrStatic_Calloc,
    StrStatic_Free,
    NULL
};


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

static void InitStringStatic (void) __attribute__((constructor(GBRUNTIME_CTOR_PRIORITY+1)));
static void ReleaseStringStatic (void) __attribute__((destructor(GBRUNTIME_CTOR_PRIORITY+1)));

static void InitStringStatic (void)
{
    DEBUG_ASSERT( StrImpl_getStringAtomsList() == NULL );
    
    if( StrImpl_getStringAtomsList() == NULL)
    {
        if(StrImpl_Initialize() == 0)
        {
            DEBUG_ASSERT(0);
        }
    }
    
    DEBUG_ASSERT( StaticStringAllocator.usrPtr == NULL );
    
    if( StaticStringAllocator.usrPtr == NULL)
    {
        
        StaticStringAllocator.usrPtr = SetInit(NULL , NULL);
        
    }
    
}
static void ReleaseStringStatic (void)
{

    if( StaticStringAllocator.usrPtr != NULL)
    {
        SetIterator*it = SetCreateIterator( StaticStringAllocator.usrPtr);

        while (SetIteratorHasNext(it) )
        {
            void* ptr = SetIteratorGetNext(it);
            
            if(SetRemoveValue(StaticStringAllocator.usrPtr, ptr) == 0)
            {
                DEBUG_LOG("Error removing static string '%s' \n" , GBStringGetCStr(ptr));
            }
            free(ptr);
            
        }
        SetIteratorFree(it);
        
        if( SetGetSize(StaticStringAllocator.usrPtr) != 0)
        {
            DEBUG_ERR("[ReleaseStringStatic] warning : some static GBStrings are still in memory (%zi remains)\n" ,SetGetSize(StaticStringAllocator.usrPtr) );
            
        }
        
        SetFree( StaticStringAllocator.usrPtr );
        StaticStringAllocator.usrPtr = NULL;
    }
}

static uint8_t String_setContent(struct _String *string , const char* content)
{
    if( content == NULL)
        return 0;

    const GBIndex index = AtomExists( content );
    
    if( index != GBIndexInvalid )
    {
        struct StringImpl* atm = CONST_CAST(struct StringImpl*  ) ArrayGetValueAtIndex(StrImpl_getStringAtomsList(), index);
        if( atm)
        {
            ++atm->count;
            string->_atom = atm;
        }
    }
    else
    {
        struct StringImpl* atm = GBMalloc(sizeof(struct StringImpl));
        
        atm->text = strdup(content);
        DEBUG_ASSERT(atm->text);

        atm->count = 1;
        ArrayAddValue( StrImpl_getStringAtomsList(), atm);
        
        string->_atom = atm;
    }
    return 1;
}

static void * String_ctor(void * _self, va_list * app)
{
    GBString *self = _self;
    
    self->_atom = NULL;
    
    const char * text = va_arg(* app, const char *);
    
    
    String_setContent( self, text);
    

    return self;
}


static void * String_dtor (void * _self)
{
    GBString *self = (void *) _self;

    if( AtomRemove(self->_atom))
    {

    }

    return  self;
}


static void * String_clone (const void * _self)
{
    const GBString * self = _self;
    
    DEBUG_ASSERT(self->_atom);
    return GBObjectAlloc(  GBDefaultAllocator ,GBStringClass, self->_atom->text);
}

static uint8_t String_equals (const void * _self, const void * _b)
{
    const GBString *self = _self;
    const GBString *b = _b;
    
    if (self == b)
        return 1;
    
    if (! b || b->base.class != GBStringClass)
        return 0;
    
    if( self->_atom->text && b->_atom->text)
    {
        return strcmp(self->_atom->text, b->_atom->text) == 0;
    }
    return 0;
}


static GBRef String_description (const void * _self)
{
    const GBString *self = (const GBString *) _self;
    
    if( self)
    {
        return GBObjectClone(self);
    }
    
    return NULL;
}


/* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */


static void* StrStatic_Malloc(GBSize size , const void *_self)
{
    UNUSED_PARAMETER(size);
    UNUSED_PARAMETER(_self);
    /* This method should NOT be used*/
    DEBUG_ASSERT(0);
    return NULL;
}
static void* StrStatic_Realloc(void *ptr, GBSize size , const void *self)
{
    UNUSED_PARAMETER(ptr);
    UNUSED_PARAMETER(size);
    UNUSED_PARAMETER(self);
    return NULL;
}
static void* StrStatic_Calloc( GBSize count, GBSize size , const void *_self)
{
    DEBUG_ASSERT(StaticStringAllocator.usrPtr != NULL);
    
    DEBUG_ASSERT( ((GBAllocator*) _self)->usrPtr == StaticStringAllocator.usrPtr);
    UNUSED_PARAMETER(_self);
    
    void* ptr = calloc( count ,size);
    
    if( SetAddValue(StaticStringAllocator.usrPtr, ptr))
    {
        return ptr;
    }
    
    return NULL;
    
}
static void  StrStatic_Free( void* ptr , const void *self)
{
    UNUSED_PARAMETER(ptr);
    UNUSED_PARAMETER(self);
    /* This method should NOT be used*/
    DEBUG_ASSERT(0);
}

/* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */
/*
    PUBLIC API
 */
/* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */



const GBString* GBStringCreateStatic(const char *format , ...)
{

    char *text = NULL;
    va_list arg;
    va_start(arg, format);
     
    const int ret = vasprintf(&text, format, arg);
    va_end(arg);

    const void* ptr = NULL;
    
    if( ret >= 0)
    {
         
        SetIterator* it = SetCreateIterator(StaticStringAllocator.usrPtr);

        while (SetIteratorHasNext(it))
        {
             const GBString* str = SetIteratorGetNext(it);
             if( GBStringEqualsCStr(str, text))
             {
                 ptr = str;
                 break;
             }
         }
        
         SetIteratorFree(it);
         
         if( ptr == NULL)
         {   
             ptr = GBObjectAlloc(  StaticStringAllocator ,GBStringClass , text);
         }
     }
     
    free(text);
    return ptr;
}

GBString* GBStringInit()
{
    return GBObjectAlloc(  GBDefaultAllocator ,GBStringClass , NULL);
}

GBString* GBStringInitWithCStr(const char *text)
{
    return GBObjectAlloc(  GBDefaultAllocator ,GBStringClass , text);
}

GBString* GBStringInitWithFormat(const char *format , ...)
{
    char *buf = NULL;
    va_list arg;
    va_start(arg, format);
    
    const int ret = vasprintf(&buf, format, arg);
    va_end(arg);
    
    if( ret >= 0)
    {
        GBString* str =  GBStringInitWithCStr(buf);
        
        free( buf );
        
        return str;
    }
    
    return NULL;
}

BOOLEAN_RETURN uint8_t GBStringSetContent(GBString* string,const char* content)
{

    if( string->_atom &&  string->_atom->text )
    {
        AtomRemove( string->_atom);
        string->_atom = NULL;
    }
    
    if( content == NULL)
        return 1;
    
    return String_setContent(string, content);
    
    //return 0;
}

BOOLEAN_RETURN uint8_t GBStringSetContentWithFormat(GBString* string, const char *format , ...)
{
    char *buf = NULL;
    va_list arg;
    va_start(arg, format);
    
    const int ret = vasprintf(&buf, format, arg);
    va_end(arg);
    
    if( ret >= 0)
    {
        const uint8_t r = GBStringSetContent( string , buf);
        
        free(buf);
        
        return r;
    }
    
    return 0;
}

BOOLEAN_RETURN uint8_t GBStringClear(GBString* string)
{
    return GBStringSetContent(string, NULL);
}

GB_PURE GBSize GBStringGetLength(const GBString* string) 
{
    if( string == NULL || string->_atom == NULL || string->_atom->text == NULL)
        return 0;
    
    return strlen( string->_atom->text );
}

const char*GBStringGetCStr(const GBString* string)
{
    if( string && string->_atom)
    {
        return string->_atom->text;
    }
    return NULL;
}

BOOLEAN_RETURN uint8_t GBStringAppendCStr(GBString* string,const char* content)
{
    if( string == NULL)
        return 0;
    
    if( content == NULL)
        return 0;
    
    const GBSize size = GBStringGetLength(string) + strlen(content);
    
    char buf[size];
    strcpy(buf, GBStringGetCStr(string));
    
    if( strncat(buf, content, size) != NULL)
    {
        return GBStringSetContent(string, buf);
    }
    return 0;
}

BOOLEAN_RETURN uint8_t GBStringAppend(GBString* string,const GBString *other)
{
    return GBStringAppendCStr(string , GBStringGetCStr(other));
}

BOOLEAN_RETURN uint8_t GBStringEquals( const GBString *str1 , const GBString *str2  )
{
    if(str1 == NULL)
        return 0;
    
    if(str2 == NULL)
        return 0;
    
    if( GBStringGetLength(str1) != GBStringGetLength(str2))
        return 0;
    
    return strcmp( GBStringGetCStr(str1), GBStringGetCStr(str2)) == 0;
}

BOOLEAN_RETURN uint8_t GBStringEqualsCStr( const GBString *str1 , const char *str2  )
{
    if(str1 == NULL)
        return 0;
    
    if(str2 == NULL)
        return 0;
    
    
    return strcmp( GBStringGetCStr(str1), str2) == 0;
}

char GBStringGetCharacterAt(const GBString* string , GBIndex index)
{
    if (string == NULL)
        return 0;
    
    return string->_atom->text[index];
}

BOOLEAN_RETURN uint8_t GBStringIsPrintable( const GBString *string )
{
    if (string == NULL)
        return 0;
    
    const char* str = string->_atom->text;
    
    for (; *str; str++)
    {
        if (!isprint((int)*str))
        {
            return 0;
        }
    }
    return 1;
    
}

BOOLEAN_RETURN uint8_t GBStringIsValid( const GBString *string )
{
    if (string == NULL)
        return 0;
    
    if( string->_atom == NULL)
        return 0;
    if( string->_atom->text == NULL) // this one should'nt occur...
        return 0;
    
    
    
    return GBStringIsPrintable(string);
}

BOOLEAN_RETURN uint8_t GBStringBeginsWith(const GBString* string , const GBString* prefix)
{
    return strncmp(GBStringGetCStr(prefix), GBStringGetCStr(string), strlen( GBStringGetCStr(prefix) ) ) == 0;
    
}

GBSize GBStringScan( const GBString* string , const char* format , ...)
{
    if( string == NULL)
        return GBSizeInvalid;
    
    if (format == NULL)
        return GBSizeInvalid;
    
    
    va_list args;
    va_start (args, format);
    const int ret = vsscanf (GBStringGetCStr(string), format, args);
    va_end (args);
    
    return (GBSize)ret;
}


GBArray* GBStringSplitString(const GBString* string, const char delimiter)
{
    if( string == NULL)
        return NULL;

//    char* stringCpy = strdup(GBStringGetCStr(string));
    
    GBArray* tokens = GBArrayInit();// = str_split( stringCpy , delimiter);
    

    char* strV = strdup( GBStringGetCStr( string));
    
    char *last = strV;
    char* t = NULL;
    
    const char sep[1] = { delimiter};
    
    while ((t = strtok_r(last, sep, &last)) != NULL)
    {
        const GBString* tt = GBStringInitWithCStr(t);
        GBArrayAddValue(tokens, tt);
        GBRelease(tt);
    }
    
    free ( strV );
    
    return tokens;
}



GBString* GBStringByRemovingChars(const GBString* string , const char* chars)
{
    if( string == NULL)
        return NULL;
    
    if( chars == NULL)
        return NULL;
    
    char* newStr = strdup(GBStringGetCStr(string));
    const GBSize numChars = strlen(chars);
    
    for(GBIndex i = 0; i < numChars ; i++)
    {
        Internal_removeAllCharOccurences(newStr , chars[i]);
    }
    
    GBString* ret = GBStringInitWithCStr(newStr);
    
    free(newStr);
    
    return ret;
}


GBHashCode GBStringHash( const GBString* string)
{
    if( string == NULL)
        return 0;
    
    return GBHashFunction(GBStringGetCStr( string), GBStringGetLength(string));
}


