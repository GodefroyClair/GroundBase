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
//  Dictionary.h
//  GroundBase
//
//  Created by Manuel Deneu on 03/09/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

/*
 Dictionary Class is for GroundBase's internal use only. Its main purpose if to serve as GBDictionary Backend.
 */

#ifndef Dictionary_h
#define Dictionary_h


#include <GBCommons.h>
#include <GBTypes.h> // GBSize

typedef struct _Dictionary Dictionary;

// return 0 to stop the iteration
typedef int (*DictionaryIterator)( const Dictionary* dict, const char* key , void* value , void* context);

/* create a new empty dictionary */
Dictionary* DictionaryInit(void);

void DictionaryFree( Dictionary* dictionary );


BOOLEAN_RETURN uint8_t DictionaryClear(Dictionary* dict );
GBSize DictionaryGetSize(const Dictionary* dict);

/* insert a new key-value pair into an existing dictionary */
BOOLEAN_RETURN uint8_t  DictionaryAddValueForKey(Dictionary* dictionary, char *key, void *value);

BOOLEAN_RETURN uint8_t  DictionaryContainsKey(const Dictionary* dictionary, const char *key);

/* return the most recently inserted value associated with a key */
/* or NULL if no matching key is present */
void *DictionaryGetValueForKey(const Dictionary* dictionary, const char *key);



/* delete the most recently inserted record with the given key */
/* if there is no such record, has no effect */
BOOLEAN_RETURN uint8_t DictionaryRemoveKey(Dictionary* dictionary, const char *key);
//BOOLEAN_RETURN uint8_t DictionaryRemoveAndFreeKey(Dictionary* dictionary, const char *key);

void DictionaryIterateValues(const Dictionary* dict , DictionaryIterator iterateMethod , void* context);

#endif /* Dictionary_h */
