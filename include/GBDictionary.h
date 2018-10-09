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
//  GBDictionary.h
//  GroundBase
//
//  Created by Manuel Deneu on 28/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//


/**
 * \file GBDictionary.h
 * \brief  C Key-based dictionary that can store GBObject values with a string key. See GBDictionary.hpp for C++ API.
 */


#ifndef GBDictionary_h
#define GBDictionary_h

#ifdef __cplusplus
extern "C" {
#endif

#include <GBCommons.h>
#include <GBObject.h>
#include <GBString.h>
#include <GBArray.h>
#include <GBSequence.h>

extern GBObjectClassRef GBDictionaryClass;
#define GBDictionaryClassName (const char*) "GBDictionary"

/*!
 * @discussion An opaque type for a dictionary.
 */
typedef struct _GBDictionary GBDictionary;

/*!
 * @discussion Initialize an empty GBDictionary instance. You own the returned object. See GBObject ownership notes.
 * @return an empty GBDictionary instance
 */
GBDictionary* GBDictionaryInit( void );

/*!
 * @discussion Add a value for a given key
 * @param dict the dictionary in which the value is added
 * @param value the value to add. The value will be retained
 * @param key the associed key.
 * @return 1 if the key/value association is added
 */
BOOLEAN_RETURN uint8_t GBDictionaryAddValueForKey(GBDictionary* dict, GBRef value ,  const GBString* key);

/*!
 * @discussion Check if a value exists for a given key
 * @param dict the dictionary in which to perform the search
 * @param key the key to search.
 * @return 1 if the value exists, 0 otherwise.
 */
BOOLEAN_RETURN uint8_t GBDictionaryContains(const GBDictionary* dict , const GBString* key);

/*!
 * @discussion Remove a value for a given key
 * @param dict the dictionary in which to remove the value.
 * @param key the key to remove
 * @return 1 if the value is removed, 0 otherwise.
 */
BOOLEAN_RETURN uint8_t GBDictionaryRemove( GBDictionary* dict , const GBString* key);

/*!
 * @discussion Get a value associated to a key
 * @param dict the dictionary in which to get the value
 * @param key the key to get
 * @return a valid Ref to the value if found, NULL otherwise. Not, you need to explicitly retain the value if you want to keep a reference.
 */
GBRef GBDictionaryGetValueForKey(const GBDictionary* dict,  const GBString *key);

/*!
 * @discussion Get the dictionary items count.
 * @param dict a valid dictionary instance
 * @return the dictionary item's count.
 */
GBSize GBDictionaryGetSize(const GBDictionary *dict);

/*!
 * @discussion Empties a dictionary. Each stored value will be released.
 * @param dict a valid dictionary instance
 * @return 1 if the operation succeded, 0 otherwise.
 */
BOOLEAN_RETURN uint8_t GBDictionaryClear( GBDictionary* dict );

/*!
 * @discussion Returns an array of keys contained in the dictionary.
 * @param dict a valid dictionary instance. Returns NULL if NULL.
 * @return an array of GBStrings or NULL if invalid. Note that you must release the returned value.
 */
GBArray* GBDictionaryGetKeyList( const GBDictionary* dict);

/*!
 * @discussion Iterates over a dictionary key/values via a callback.
 * @param dict a valid dictionary instance. Will do nothing if NULL.
 * @param method This method will be called for each key/value pair in the dictionary. Will do nothing if NULL.
 * @param context A user pointer to pass with each call to iterateMethod. Can be NULL.
 */
void GBDictionaryIterateValues( const GBDictionary* dict , GBSequenceIterator method , void* context);

#ifdef __cplusplus
}
#endif
#endif /* GBDictionary_h */
