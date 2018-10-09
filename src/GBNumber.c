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
//  GBNumber.c
//  GroundBase
//
//  Created by Manuel Deneu on 28/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

/* 
 We need this on Raspberry pi ( ARM issue?) for asprintf to properly work. 
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>

#include <string.h> // memset
#include <GBNumber.h>

#include "GBObject_Private.h"
#include "GBNumber_Private.h"

static void * Number_ctor(void * _self, va_list * app);
static void * Number_dtor (void * _self);
static void * Number_clone (const void * _self);
static uint8_t  Number_equals (const void * _self, const void * _b);
static GBRef Number_description (const void * self);



struct Number
{
    GBObjectBase base;
    
    struct _GBNumberValue impl;
    
};

static const GBObjectClass _NumberClass =
{
    sizeof(struct Number),
    Number_ctor,
    Number_dtor,
    Number_clone,
    Number_equals,
    Number_description,
    NULL, // initialize
    NULL, // deinit
    //serialize,
    
    NULL, //retain
    NULL, // release
    (char*)GBNumberClassName
};


GBObjectClassRef GBNumberClass = & _NumberClass;

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

static void * Number_ctor(void * _self, va_list * app)
{
    struct Number * self = _self;
    
    const GBNumberType type = va_arg(*app, GBNumberType);
    self->impl.type = type;
    
    switch (type)
    {
        case GBNumberTypeInt:
            self->impl.value.intVal = va_arg(*app++, int);
            break;
            
        case GBNumberTypeFloat:
            self->impl.value.floatVal = (float)va_arg(*app++, double);
            break;
            
        case GBNumberTypeDouble:
            self->impl.value.doubleVal = va_arg(*app++, double);
            break;
            
        case GBNumberTypeLong:
            self->impl.value.longVal = va_arg(*app++, long);
            break;
            
        default:
            self->impl.type = GBNumberTypeUnknown;
            memset(&self->impl.value , 0 , sizeof(self->impl.value));
            break;
    }
    
    
    return self;
}
static void * Number_dtor (void * _self)
{
    struct Number * self = (void *) _self;
    return self;
}



static void * Number_clone (const void * _self)
{
    const struct Number * self = _self;
    
    struct Number *cpy = GBObjectAlloc( GBDefaultAllocator,GBNumberClass);
    
    cpy->impl.type = self->impl.type;
    
    memcpy(&cpy->impl.value, &self->impl.value, sizeof(self->impl.value));
    
    return cpy;
    
}
static uint8_t Number_equals (const void * _self, const void * _b)
{
    const struct Number * self = _self;
    const struct Number * b = _b;

    /*
    if( GBNumberGetType(self) != GBNumberGetType(b))
        return 0;
    */
    switch ( GBNumberGetType(self) )
    {
        case GBNumberTypeInt:
            return GBNumberGetInt(self) == GBNumberToInt(b);
            
        case GBNumberTypeDouble:
            return GBNumberGetDouble(self) == GBNumberToDouble(b);
        case GBNumberTypeFloat:
            return GBNumberGetFloat(self) == GBNumberToFloat(b);
        case GBNumberTypeLong:
            return GBNumberGetLong(self) == GBNumberToLong(b);
            
        default:
            break;
    }
    
    return 0;// self->impl.type == b->impl.type && self->impl.value.doubleVal == b->impl.value.doubleVal;
    
}





static GBRef Number_description (const void * _self)
{
    
    const GBNumber* self = (const GBNumber* ) _self;
    if( self)
    {
        const GBString* ret = NULL;
        switch (GBNumberGetType(self))
        {
            case GBNumberTypeInt:
                
                ret = GBStringInitWithFormat("Int Value %i" , GBNumberGetInt(self));
                break;
                
            case GBNumberTypeDouble:
                ret = GBStringInitWithFormat("Double Value %f" , GBNumberGetDouble(self));
                break;
                
            case GBNumberTypeFloat:
                ret = GBStringInitWithFormat("Float Value %f" , GBNumberGetFloat(self));
                break;
                
            case GBNumberTypeLong:
                ret = GBStringInitWithFormat("Long Value %ld" , GBNumberGetLong(self));
                break;
                
            default:
                ret = GBStringInitWithFormat("Unknown type %i" , GBNumberGetType(self));
                break;
        }
        if( ret)
        {
            return ret;
        }
        DEBUG_ASSERT(0);
            
    
    }
    return NULL;
}

GBNumber* GBNumberInit()
{
    GBNumber *num  = GBObjectAlloc( GBDefaultAllocator,GBNumberClass);
    
    return num;
}

GBNumber* GBNumberInitWithFloat(float value)
{
    return GBObjectAlloc( GBDefaultAllocator,GBNumberClass ,GBNumberTypeFloat, value);
}

GBNumber* GBNumberInitWithInt(int value)
{
    return GBObjectAlloc( GBDefaultAllocator,GBNumberClass , GBNumberTypeInt , value);
}

GBNumber* GBNumberInitWithLong(long value)
{
    return GBObjectAlloc( GBDefaultAllocator,GBNumberClass , GBNumberTypeLong , value);
}

GBNumber* GBNumberInitWithDouble(double value)
{
    return GBObjectAlloc( GBDefaultAllocator,GBNumberClass , GBNumberTypeDouble , value);
}


/* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */
/* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */


BOOLEAN_RETURN uint8_t GBNumberSetInt( GBNumber* self , int value)
{
    if(self)
    {
        self->impl.value.intVal = value;
        self->impl.type = GBNumberTypeInt;
        return 1;
    }
    
    return 0;
}

BOOLEAN_RETURN uint8_t GBNumberSetDouble( GBNumber* self , double value)
{

    if(self)
    {
        self->impl.value.doubleVal = value;
        self->impl.type = GBNumberTypeDouble;
        return 1;
    }
    
    return 0;
}

BOOLEAN_RETURN uint8_t GBNumberSetFloat( GBNumber* self , float value)
{

    if(self)
    {
        self->impl.value.floatVal = value;
        self->impl.type = GBNumberTypeFloat;
        return 1;
    }
    
    return 0;
}
BOOLEAN_RETURN uint8_t GBNumberSetLong( GBNumber* self , long value)
{

    if(self)
    {
        self->impl.value.longVal = value;
        self->impl.type = GBNumberTypeLong;
        return 1;
    }
    
    return 0;
}

/* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */
/* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */

GBNumberType GBNumberGetType(const GBNumber* self)
{
    if( self)
    {
        return self->impl.type;
    }
    return GBNumberTypeUnknown;
}

/* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */

float GBNumberGetFloat(const GBNumber* self)
{
    return self->impl.value.floatVal;
}

int GBNumberGetInt(const GBNumber* self)
{
    return self->impl.value.intVal;
}

double GBNumberGetDouble(const GBNumber* self)
{
    return self->impl.value.doubleVal;
}
long GBNumberGetLong(const GBNumber* self)
{
    return self->impl.value.longVal;
}

// private! defined in GBNumber_Private
const struct _GBNumberValue *GBNumberGetImpl(const GBRef number)
{
    return &((GBNumber*)number)->impl;
}

/* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */

int GBNumberToInt( const GBNumber* self)
{
    switch (self->impl.type)
    {
        case GBNumberTypeInt:
            return GBNumberGetInt(self);
        case GBNumberTypeDouble:
            return (int) GBNumberGetDouble(self);
        case GBNumberTypeFloat:
            return (int) GBNumberGetFloat(self);
        case GBNumberTypeLong:
            return (int) GBNumberGetLong(self);
        case GBNumberTypeUnknown:
        default:
            DEBUG_ASSERT(0);
            break;
    }
    
    return 0;
}

float GBNumberToFloat(const GBNumber* self)
{
    switch (self->impl.type)
    {
        case GBNumberTypeInt:
            return (float)GBNumberGetInt(self);
        case GBNumberTypeDouble:
            return (float) GBNumberGetDouble(self);
        case GBNumberTypeFloat:
            return GBNumberGetFloat(self);
        case GBNumberTypeLong:
            return (float) GBNumberGetLong(self);
        case GBNumberTypeUnknown:
        default:
            DEBUG_ASSERT(0);
            break;
    }
    
    return 0.f;
}
double GBNumberToDouble(const GBNumber* self)
{
    switch (self->impl.type)
    {
        case GBNumberTypeInt:
            return (double)GBNumberGetInt(self);
        case GBNumberTypeDouble:
            return GBNumberGetDouble(self);
        case GBNumberTypeFloat:
            return (double) GBNumberGetFloat(self);
        case GBNumberTypeLong:
            return (double) GBNumberGetLong(self);
        case GBNumberTypeUnknown:
        default:
            DEBUG_ASSERT(0);
            break;
    }
    
    return 0.;
}
long GBNumberToLong(const GBNumber* self)
{
    switch (self->impl.type)
    {
        case GBNumberTypeInt:
            return (long) GBNumberGetInt(self);
        case GBNumberTypeDouble:
            return (long) GBNumberGetDouble(self);
        case GBNumberTypeFloat:
            return (long) GBNumberGetFloat(self);
        case GBNumberTypeLong:
            return  GBNumberGetLong(self);
        case GBNumberTypeUnknown:
        default:
            DEBUG_ASSERT(0);
            break;
    }
    
    return 0.;
}

char* GBNumberGetString( const GBNumber *number)
{
    if( number == NULL)
    {
        return NULL;
    }
    
    char* b = NULL;
    switch (number->impl.type)
    {
        case GBNumberTypeInt:
            if(asprintf(&b, "%i" , GBNumberGetInt(number)) > 0)
            {
                return b;
            }
            break;
        case GBNumberTypeDouble:
            if(asprintf(&b, "%f" , GBNumberGetDouble(number)) > 0)
            {
                return b;
            }
            break;
            
        case GBNumberTypeFloat:
            if( asprintf(&b, "%f" , GBNumberGetFloat(number)) > 0)
            {
               return b;
            }
            break;
            
        case GBNumberTypeLong:
            if(asprintf(&b, "%li" , GBNumberGetLong(number)) > 0)
            {
                return b;
            }
            break;
        case GBNumberTypeUnknown:
        default:
            DEBUG_ASSERT(0);
            break;
    }
    
    return NULL;
}
