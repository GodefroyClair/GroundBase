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
//  GBString.h
//  GroundBase
//
//  Created by Manuel Deneu on 28/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//


/**
 * \file GBString.h
 * \brief GBString is a c-string wrapper providing a strong and safe type for string operations.
 * see GB::String in GBString.hpp for c++ Counterpart.
 */

#ifndef GBString_h
#define GBString_h

#ifdef __cplusplus
extern "C" {
#endif

#include <GBObject.h>
#include <GBContainer.h>
#include <GBArray.h>

extern GBObjectClassRef GBStringClass;
#define GBStringClassName (const char*) "GBString"

/*!
 * @discussion An opaque GBString instance.
 */
typedef struct _String GBString;



/* Constructors */

/*!
 * @discussion Creates an empty string
 * @return An empty instance of GBString
 */
GBString* GBStringInit(void);



/*!
 * @discussion Creates an string
 * @param text A C-String.
 * @return An instance of GBString containing the input text.
 */
GBString* GBStringInitWithCStr(const char *text);

/*
 Creates an immutable string, or NULL if there was a problem creating the object. 
 The returned object is a constant. You may retain and release it, similar to other GBString objects,
 but are not required to do so—it will remain valid until the program terminates.
 */
    
/*!
 * @discussion Creates an immutable string, or NULL if there was a problem creating the object.
 * @param format A format String.
 * @param ... A comma-separated list of arguments to substitute into format.
 * @return An instance of GBString containing the input text.
 
 The returned object is a constant. You may retain and release it, similar to other GBString objects,
 but are not required to do so—it will remain valid until the program terminates.
 */
const GBString* GBStringCreateStatic(const char *format , ...);

/* Just an alias, to save some time typing ...*/
#define GBSTR(char,...) GBStringCreateStatic(char , ##__VA_ARGS__ )

/*!
 * @discussion returns a string created by using a given format string as a template into which the remaining argument values are substituted.
 * @param format A format String.
 * @param ... A comma-separated list of arguments to substitute into format.
 * @return An instance of GBString containing the format string & arguments
 */
GBString* GBStringInitWithFormat(const char *format , ...);

/*!
 * @discussion returns a c string pointer
 * @param string A GBString instance.
 * @return a read-only pointer to the underneath c-string, or NULL if string is invalid.
 */
const char*GBStringGetCStr(const GBString* string);

/*!
 * @discussion returns the character at index.
 * @param string A GBString instance.
 * @param index a valid index. This index is not checked so undefined behaviour if index is out of bounds.
 * @return a character, or 0 if invalid.
 */
char GBStringGetCharacterAt(const GBString* string , GBIndex index);

/* Getters */

/*!
 * @discussion returns the length of the string.
 * @param string A GBString instance.
 * @return The Length of the string.
 */
GBSize GBStringGetLength(const GBString* string);

/*!
 * @discussion Check is the string is empty, i.e equals to "".
 * @param string A GBString instance.
 * @return 1 if empty, 0 if not.
 */
static inline BOOLEAN_RETURN uint8_t GBStringIsEmpty(const GBString* string)
{
    return GBStringGetLength(string) == 0;
}

/*!
 * @discussion Checks if two GBString instances are equals.
 * @param str1 A GBString instance.
 * @param str2 A GBString instance.
 * @return 1 if str1 === str2 
 */
BOOLEAN_RETURN uint8_t GBStringEquals( const GBString *str1 , const GBString *str2  );

/*!
 * @discussion Checks if a GBString instance if equals to a C-string. This method is equivalent to 'strlen( GBStringGetCStr(str1) , str2) == 0'
 * @param str1 A GBString instance.
 * @param str2 A c-string instance
 * @return 1 if str1 === str2.
 */
BOOLEAN_RETURN uint8_t GBStringEqualsCStr( const GBString *str1 , const char *str2  );

/*!
 * @discussion Checks if a GBString instance if printable. see 'man isprint' to more informations.
 * @param string A GBString instance.
 * @return 1 if the string if printable.
 */
BOOLEAN_RETURN uint8_t GBStringIsPrintable( const GBString *string );

/*!
 * @discussion Checks if a GBString instance if valid, ie is not NULL and contains text. Mostly equivalent to `GBStringIsEmpty == 0`
 * @param string A GBString instance.
 * @return 1 if the string if valid.
 */
BOOLEAN_RETURN uint8_t GBStringIsValid( const GBString *string );

/*-- Edition --*/

/*!
 * @discussion Appends content to a GBString from a C string.
 * @param string A GBString instance to append to. Will return 0 if NULL.
 * @param content A c string valid pointer to append from. Will return 0 if NULL.
 * @return 1 if the concatenation succedeed.
 */
BOOLEAN_RETURN uint8_t GBStringAppendCStr(GBString* string,const char* content);
    
/*!
 * @discussion Appends content to a GBString from a GBString string.
 * @param string A GBString instance to append to. Will return 0 if NULL.
 * @param other A GBString instance to append from. Will return 0 if NULL.
 * @return 1 if the concatenation succedeed.
 */
BOOLEAN_RETURN uint8_t GBStringAppend(GBString* string,const GBString *other);
    
/*!
 * @discussion Overwrites the content of a GBString from a C String.
 * @param string A GBString instance. Undefined behaviour if NULL
 * @param content A c string pointer. if NULL, this call is equivalent to 'GBStringClear', if not NULL the string will be copied.
 * @return 1 on sucess, 0 on error.
 */
BOOLEAN_RETURN uint8_t GBStringSetContent(GBString* string,const char* content) GB_NO_NULL_ARGS(1,1);
   
/*!
 * @discussion Overwrites the content of a GBString from a C String format.
 * @param string A GBString instance. Undefined behaviour if NULL
 * @param format A c string format followed by arguments. Undefined behaviour if NULL.
 * @return 1 on sucess, 0 on error.
 */
BOOLEAN_RETURN uint8_t GBStringSetContentWithFormat(GBString* string,const char *format , ...)GB_NO_NULL_POINTERS;
    
/*!
 * @discussion Empties the string. Equivalent to GBStringSetContent(str , NULL)
 * @param string A GBString instance. Undefined behaviour if NULL
 * @return 1 on sucess, 0 on error.
 */
BOOLEAN_RETURN uint8_t GBStringClear(GBString* string) GB_NO_NULL_POINTERS;

/*!
 * @discussion Checks if a string begins with an other string prefix.
 * @param string A GBString instance to test. Undefined behaviour if NULL
 * @param prefix A GBString prefix to find a the beginning of 'instance'. Undefined behaviour if NULL
 * @return 1 if 'intance‘ begins with 'prefix‘ , 0 on error.
 */
BOOLEAN_RETURN uint8_t GBStringBeginsWith(const GBString* string , const GBString* prefix) GB_NO_NULL_POINTERS;

    
/*!
 * @discussion Performs a scanf-like operation on a GBString. See man 2 scanf for more informations.
 * @param string A GBString instance to test. Returns GBSizeInvalid if NULL
 * @param format A c string format followed by arguments. Returns GBSizeInvalid if NULL
 * @return the number of elements found in the string
 */
GBSize GBStringScan( const GBString* string , const char* format , ... ) GB_SCANF_LIKE(2,3);


/*!
 * @discussion Splits a string according to a char delimiter
 * @param string A GBString instance to split. Will return NULL if NULL.
 * @param delimiter a char delimiter
 * @return an array containing the string tokens found. The returned object needs to be released!
 */
GBArray* GBStringSplitString(const GBString* string, const char delimiter);

    
/*!
 * @discussion Finds and removes chars from a string and returns the modified string. The string passed as parameter will be left untouched.
 * @param string A GBString instance. Will not be modified by the method. Will return NULL if NULL.
 * @param chars a list of char to remove. Will return NULL if NULL.
 * @return A new copy of 'instance' string, minus the found characters from 'chars'.The returned object needs to be released!
 */
GBString* GBStringByRemovingChars(const GBString* string , const char* chars);
/* Debug*/

    
/*!
 * @discussion Hash a string
 * @param string A GBString instance. Will return 0 if NULL.
 * @return A hash of the string, or 0 if invalid.
 */
GBHashCode GBStringHash( const GBString* string);

#ifdef __cplusplus
}
#endif


#endif /* GBString_h */
