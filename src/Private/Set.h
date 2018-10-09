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

#ifndef Set_h
#define Set_h

#include <GBCommons.h>
#include <GBTypes.h>

typedef enum
{
    IterationMode_Preorder,
    IterationMode_Inorder,
    IterationMode_Postorder
} IterationMode;

typedef struct _Set Set;

/* Returns 1 is objects differs */
typedef int64_t (*SetComparator)( const Set *self, void *a, void *b);
typedef void* (*SetValueGetKey)( const Set *self, void *value);
typedef BOOLEAN_RETURN uint8_t (*SetIteratorCallback)( const Set* set , const void *value , void* context);


Set* SetInit( SetComparator comparator , SetValueGetKey getKey );

void SetFree( Set* set);

GBSize SetGetSize(const Set* set);

BOOLEAN_RETURN uint8_t SetAddValue( Set* set , void* value );

BOOLEAN_RETURN uint8_t SetContainsValue( const Set* set ,void* value );

BOOLEAN_RETURN uint8_t SetRemoveValue( Set* set ,void* value );

BOOLEAN_RETURN uint8_t SetIsEmpty( const Set* set  );
BOOLEAN_RETURN uint8_t SetClear( Set* set  );

BOOLEAN_RETURN uint8_t SetIterateValues( const Set* set , SetIteratorCallback function , void *context  );

/* **** */

typedef struct siterator SetIterator;

SetIterator* SetCreateIterator( const Set* set);
BOOLEAN_RETURN uint8_t SetIteratorHasNext( SetIterator* it);
void* SetIteratorGetNext( SetIterator* it );
void SetIteratorFree(SetIterator* it);

#endif /* Set_h */
