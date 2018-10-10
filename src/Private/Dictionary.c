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
//  Dictionary.c
//  GroundBase
//
//  Created by Manuel Deneu on 03/09/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#include <stdio.h>

#include <string.h>
#include "LibUt/uthash.h"

#include "Dictionary.h"
#include <GBAllocator.h>


typedef struct
{
    char* key;
    void* value;
    UT_hash_handle hh;
} Entry;

struct _Dictionary
{
    Entry *head;
};

Dictionary* DictionaryInit(void)
{
    Dictionary* self = GBMalloc(sizeof(Dictionary));
    
    if( self)
    {
        self->head = NULL;
        
        return self;
    }
    return NULL;
}

void DictionaryFree( Dictionary* dictionary  )
{
    GBFree(dictionary);
}


BOOLEAN_RETURN uint8_t DictionaryClear(Dictionary* dict )
{
    Entry* el = NULL;
    Entry* tmp = NULL;
    HASH_ITER(hh, dict->head, el, tmp)
    {
        HASH_DEL(dict->head, el);
        free(el->key);
        GBFree(el);
        
    }
    return DictionaryGetSize(dict) == 0;
}

GBSize DictionaryGetSize(const Dictionary* dict)
{
    return (GBSize) HASH_COUNT(dict->head);
}


/* insert a new key-value pair into an existing dictionary */
BOOLEAN_RETURN uint8_t  DictionaryAddValueForKey(Dictionary* d, char *key, void *value)
{
    Entry* f= NULL;
    HASH_FIND_STR(d->head, key, f);
    
    if( f != NULL)
        return 0;
    
    Entry *e = GBMalloc(sizeof(Entry));
    e->value = value;
    e->key  = strdup(key);
    
    HASH_ADD_STR(d->head, key, e);
    return 1;
}

BOOLEAN_RETURN uint8_t  DictionaryContainsKey(const Dictionary* dictionary, const char *key)
{
    return DictionaryGetValueForKey(dictionary , key) != NULL;
}


void *DictionaryGetValueForKey(const Dictionary* d, const char *key)
{
    if( d == NULL )
        return NULL;
    
    if( d->head == NULL)
        return NULL;
    
    Entry *e = NULL;
    HASH_FIND_STR(d->head, key, e);
    
    if (e)
        return e->value;
    
    return NULL;
}

void DictionaryIterateValues(const Dictionary* dict , DictionaryIterator iterateMethod ,void* context)
{
    if( dict == NULL || iterateMethod == NULL)
        return;
        
    Entry* el = NULL;
    Entry* tmp = NULL;
    HASH_ITER(hh, dict->head, el, tmp)
    {
        if( iterateMethod(dict , el->key , el->value , context) == 0) // early exit from the iteration
            return;
        
    }
    
}

BOOLEAN_RETURN uint8_t DictionaryRemoveKey(Dictionary* dict, const char *key)
{
    Entry *f = NULL;
    HASH_FIND_STR(dict->head, key, f);
    
    if( f)
    {
        HASH_DEL(dict->head, f);
        free(f->key);
        GBFree(f);
        
        return 1;
    }
    return 0;
}
