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
//  List.c
//  GroundBase
//
//  Created by Manuel Deneu on 28/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//
/*
 Set Class is for GroundBase's internal use only. Its main purpose if to serve as GBList Backend.
 */

#include <GBCommons.h>
#include "List.h"
#include <GBAllocator.h>
#include "LibUt/utlist.h"


struct Node
{
    void* data;
    struct Node* next;
    struct Node* prev;
};

static struct Node* getNewNode( void* x);

struct _List
{
    struct Node *head;
};

//Creates a new Node and returns pointer to it.
static struct Node* getNewNode(void* x)
{
    struct Node* newNode = ( struct Node*) GBMalloc(sizeof(struct Node ) );
    if( newNode)
    {
        newNode->data = x;
        newNode->prev = NULL;
        newNode->next = NULL;
    }
    return newNode;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

List* ListInit()
{
    List* list = (List* ) GBMalloc( sizeof( List ) );
    if (list )
    {
        list->head = NULL;
    }
    
    return list;
}

void ListFree(List *list)
{
    if( list )
    {
        ListRemoveAll( list );
        
        DEBUG_ASSERT(list);
        GBFree(list);
    }
}

GBSize ListGetSize(const List* list)
{
    
    if( list == NULL )
        return 0;
    
    struct Node* el = NULL;
    int counter = 0;
    DL_COUNT( list->head, el, counter);
    
    return (GBSize) counter;
    
    
}
BOOLEAN_RETURN uint8_t ListAddValue( List* list ,void*  item)
{
    if( list == item)
        return 0;

    if( list  == 0)
        return 0;

    struct Node* newNode = getNewNode(item);
    
    DEBUG_ASSERT(newNode);
    
    newNode->data = item;
    
    DL_APPEND( list->head, newNode);
    
    return 1;
    
}

BOOLEAN_RETURN uint8_t ListRemoveValueAtIndex( List* list , GBIndex index)
{
    if( list == NULL)
        return 0;
    
    struct Node* el = NULL;
    struct Node* tmp = NULL;
    
    GBIndex i = 0;
    
    DL_FOREACH_SAFE(list->head, el, tmp )
    {
        if( i == index)
        {
            DL_DELETE(list->head, el);
            GBFree(el);
            return 1;
        }
        i++;
    }
    
    
    return 0;
}

BOOLEAN_RETURN uint8_t ListRemove( List* list , const void* item)
{
    if( list == NULL)
        return 0;
    
    struct Node* el = NULL;
    struct Node* tmp = NULL;

    DL_FOREACH_SAFE(list->head, el, tmp )
    {
        if( el->data == item)
        {
            DL_DELETE(list->head, el);
            GBFree(el);
            
            return 1;
        }
    }

    return 0;
}

BOOLEAN_RETURN uint8_t ListRemoveIter( List* list , ListIterator* iter)
{
    if( list == NULL || iter == NULL)
        return 0;
    DL_DELETE(list->head, iter);
    
    return 1;
}

BOOLEAN_RETURN uint8_t ListRemoveAll( List* list )
{
    if( list == NULL)
        return 0;
    
    
    if( ListGetSize( list ) == 0)
        return 1;
    
    struct Node* tmp = NULL;
    struct Node* del = NULL;
    DL_FOREACH_SAFE(list->head,del,tmp)
    {
        DL_DELETE(list->head,del);
        
        GBFree(del);
    }
    DEBUG_ASSERT(list->head == NULL);
    DEBUG_ASSERT(list);
    
    
    return 1;
}

GBIndex ListFind( const List *list , const void* value)
{
    if( list == NULL)
        return GBIndexInvalid;
    
    GBIndex i = 0;
    struct Node* el = NULL;
    
    DL_FOREACH(list->head, el)
    {
        if( el->data == value )
            return i;
        
        i++;
    }
    
    return GBIndexInvalid;
    
}

BOOLEAN_RETURN uint8_t ListReplaceValueAtIndex( List* list  , GBIndex index , void* newValue)
{
    if( list == NULL)
        return 0;
    
    
    struct Node* el = NULL;
    struct Node* tmp = NULL;
    
    GBIndex i = 0;
    
    DL_FOREACH_SAFE( list->head, el, tmp )
    {
        if( i == index)
        {
            el->data = newValue;
            return 1;
        }
        i++;
    }
    
    
    return 0;
}

void* ListGetValueAtIndex( const List *list , GBIndex index)
{
    if( list == NULL)
        return NULL ;
    
    struct Node* node = NULL;
    
    GBIndex i = 0;
    DL_FOREACH( list->head, node)
    {
        DEBUG_ASSERT( node );
        if( i == index)
            return node->data;
        
        i++;
    }
    
    return NULL;
}

BOOLEAN_RETURN uint8_t ListContains(const List* list , const void* item)
{
    if( list == NULL)
        return 0;
    
    struct Node* s = NULL;
    DL_SEARCH_SCALAR( list->head, s, data, item);
    
    return s != NULL;
    
}




ListIterator* ListBegin( const List* list)
{
    if ( list == NULL)
        return 0;
    
    return list->head;
}

ListIterator* ListGetNext( const ListIterator* iter )
{
    if( iter == NULL)
        return NULL;

    return iter->next;
}

void* ListGetValue( const ListIterator* iter)
{
    if( iter == NULL)
        return NULL;
    
    return iter->data;
}
