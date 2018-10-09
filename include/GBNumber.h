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
//  GBNumber.h
//  GroundBase
//
//  Created by Manuel Deneu on 28/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//


/**
 * \file GBNumber.h
 * \brief GBNumber is a wrapper for many numeric values.
 */

#ifndef GBNumber_h
#define GBNumber_h

#ifdef __cplusplus
extern "C" {
#endif

#include <GBObject.h>


extern GBObjectClassRef GBNumberClass;
#define GBNumberClassName (const char*) "GBNumber"

/*!
 * @discussion An opaque GBNumber instance.
 */
typedef struct Number GBNumber;


/*!
 * @typedef GBNumberType
 * @brief Defines the types a GBNumber can be.
 */
typedef enum
{
    GBNumberTypeUnknown = 0,
    GBNumberTypeInt     = 1,
    GBNumberTypeDouble  = 2,
    GBNumberTypeFloat   = 3,
    GBNumberTypeLong    = 4
  
} GBNumberType;


/*!
 * @discussion Initialize an empty GBNumber instance. You own the returned object. See GBObject ownership notes.
 * @return an empty GBNumber instance
 */
GBNumber* GBNumberInit(void);

/*!
 * @discussion Creates an GBNumber instance an initializes it with a float value
 * @param value A float value to store
 * @return An instance of GBNumber containing the input value.
 */
GBNumber* GBNumberInitWithFloat(float value);

/*!
 * @discussion Creates an GBNumber instance an initializes it with an int value
 * @param value An int value to store
 * @return An instance of GBNumber containing the input value.
 */
GBNumber* GBNumberInitWithInt(int value);

/*!
 * @discussion Creates an GBNumber instance an initializes it with a long value
 * @param value A long value to store
 * @return An instance of GBNumber containing the input value.
 */
GBNumber* GBNumberInitWithLong(long value);

/*!
 * @discussion Creates an GBNumber instance an initializes it with a double value
 * @param value A double value to store
 * @return An instance of GBNumber containing the input value.
 */
GBNumber* GBNumberInitWithDouble(double value);

/*!
 * @discussion Sets an GBNumber instance's value with an int value
 * @param self A valid GBNumber instance.
 * @param value An int value to store
 * @return 1 if the operation succeded.
 */
BOOLEAN_RETURN uint8_t GBNumberSetInt( GBNumber* self , int value);

/*!
 * @discussion Sets an GBNumber instance's value with a double value
 * @param self A valid GBNumber instance.
 * @param value A double value to store
 * @return 1 if the operation succeded.
 */
BOOLEAN_RETURN uint8_t GBNumberSetDouble( GBNumber* self , double value);

/*!
 * @discussion Sets an GBNumber instance's value with a float value
 * @param self A valid GBNumber instance.
 * @param value A float value to store
 * @return 1 if the operation succeded.
 */
BOOLEAN_RETURN uint8_t GBNumberSetFloat( GBNumber* self , float value);

/*!
 * @discussion Sets an GBNumber instance's value with a long value
 * @param self A valid GBNumber instance.
 * @param value A long value to store
 * @return 1 if the operation succeded.
 */
BOOLEAN_RETURN uint8_t GBNumberSetLong( GBNumber* self , long value);

/*!
 * @discussion Queries the GBNumber type of an instance
 * @param self A valid GBNumber instance.
 * @return the GBNumberType.
 */
GBNumberType GBNumberGetType(const GBNumber* self);

/*
 
 Note about GBNumberGet<Type> and GBNumberTo<Type> methods below :
  - GBNumberGet<Type> methods will not cast numbers to an other type than theirs.
    Eg :
    GBNumber* num = GBNumberInitWithFloat(12.34f);
    const int wrongVal = GBNumberGetInt(num);
    const float rightVal = GBNumberGetInt(num);
    -> wrongVal value will be unspecifier since num type is float.
    -> rightVal value will be num's value (ie 12.34)
 
 - To Get a number value in a other numeric format, use GBNumberTo<Type> to perform the cast.
 
 
 */
/*!
 * @discussion Returns the int value
 * @param self A valid GBNumber instance.
 * @return the int value. Undefined if self is not a GBNumberTypeInt type. Use GBNumberGetType first.
 */
int GBNumberGetInt(const GBNumber* self);

/*!
 * @discussion Returns the float value
 * @param self A valid GBNumber instance.
 * @return the float value. Undefined if self is not a GBNumberTypeFloat type. Use GBNumberGetType first.
 */
float GBNumberGetFloat(const GBNumber* self);

/*!
 * @discussion Returns the double value
 * @param self A valid GBNumber instance.
 * @return the double value. Undefined if self is not a GBNumberTypeDouble type. Use GBNumberGetType first.
 */
double GBNumberGetDouble(const GBNumber* self);

/*!
 * @discussion Returns the long value
 * @param self A valid GBNumber instance.
 * @return the long value. Undefined if self is not a GBNumberTypeLong type. Use GBNumberGetType first.
 */
long GBNumberGetLong(const GBNumber* self);

/* Use a C-cast type convertion */
int    GBNumberToInt( const GBNumber* self);
float  GBNumberToFloat(const GBNumber* self);
double GBNumberToDouble(const GBNumber* self);
long   GBNumberToLong(const GBNumber* self);

// needs free
char* GBNumberGetString( const GBNumber *number);

#ifdef __cplusplus
}
#endif
#endif /* GBNumber_h */
