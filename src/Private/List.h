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
//  List.h
//  GroundBase
//
//  Created by Manuel Deneu on 28/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#ifndef List_h
#define List_h

#include <stdio.h>
#include <GBCommons.h>
#include <GBTypes.h>

/*
    Set : Basic container.
    Rule : this set only holds references from external objects
    Everything is const!
 */

typedef struct _List List;

List* ListInit(void);

void ListFree( List *list);

GBSize ListGetSize(const List* list);

BOOLEAN_RETURN uint8_t ListAddValue(List* list , void* item);

BOOLEAN_RETURN uint8_t ListRemoveValueAtIndex( List* list , GBIndex index);

BOOLEAN_RETURN uint8_t ListRemove( List* list , const void* item);

/*
 Returns 0 if invalid set
 1 if succeded, even if set is empty
 */
BOOLEAN_RETURN uint8_t ListRemoveAll( List* list );
GBIndex ListFind( const List *list , const void* value);

GB_DEPRECATED("5.3.6") BOOLEAN_RETURN uint8_t ListReplaceValueAtIndex( List* list  , GBIndex index , void* newValue);

GB_DEPRECATED("5.3.6") void*   ListGetValueAtIndex( const List *list , GBIndex index);
BOOLEAN_RETURN uint8_t ListContains(const List* list , const void* item);

typedef struct Node ListIterator;
ListIterator* ListBegin( const List* list);
ListIterator* ListGetNext( const ListIterator* iter);
void* ListGetValue( const ListIterator* iter);

BOOLEAN_RETURN uint8_t ListRemoveIter( List* list , ListIterator* iter);

#define ListForEach( list , value) \
for(  ListIterator* COMBINE(__iter , __LINE__) = ListBegin(list) ;\
COMBINE(__iter , __LINE__) != NULL && (value = ListGetValue( COMBINE(__iter , __LINE__) ));\
COMBINE(__iter , __LINE__) = ListGetNext( COMBINE(__iter , __LINE__) ) )

#endif /* List_h */
