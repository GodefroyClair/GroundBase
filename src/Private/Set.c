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


#include "Set.h"
#include "RedBlack/rbtree.h"
#include <GBAllocator.h>


static void* Internal_GetPointer(tree_root *tree ,tree_node* node);
static int64_t Internal_Comparator( tree_root *tree , void* keyA, void* keyB);

struct _Set
{
    tree_root *tree;
    GBSize _size;
    SetComparator _comp;
    SetValueGetKey _getKeyFunc;
};



static void* Internal_GetPointer(tree_root *tree ,tree_node* node)
{
    DEBUG_ASSERT(tree);
    Set* self = tree->user;
    DEBUG_ASSERT(self);
    
    
    if( self && self->_getKeyFunc)
        return self->_getKeyFunc( self ,node->node);
    
    return node->node;
}



static int64_t Internal_Comparator( tree_root *tree , void* keyA, void* keyB)
{
    DEBUG_ASSERT(tree);
    Set* self = tree->user;
    DEBUG_ASSERT(self);
    
    if( self && self->_comp)
        return self->_comp( self , keyA , keyB);
    
    return (char*)keyA - (char*) keyB;
}



Set* SetInit( SetComparator comparator , SetValueGetKey getKey )
{
    Set *set = GBMalloc(sizeof( Set ));
    if (set)
    {
        set->tree =  new_rbtree( Internal_GetPointer  , Internal_Comparator );

        set->_comp = comparator;
        set->_getKeyFunc = getKey;
        set->_size = 0;
        set->tree->user = set;
        return set;

    }
    return NULL;
}

void SetFree( Set* set)
{
    if( set == NULL)
        return ;
    

    destroy_rbtree( set->tree );
    
    GBFree( set);
}

GBSize SetGetSize(const Set* set)
{
    return set->_size;
}

BOOLEAN_RETURN uint8_t SetAddValue( Set* set , void* value )
{
    if( set == NULL)
        return 0;
 
    /*
    if( SetContainsValue(set, value))
        return 0;
    */
    if( rb_tree_insert(set->tree, value) == NULL)
    {
        set->_size++;
        
        return 1;
    }
    return 0;
    
}

BOOLEAN_RETURN uint8_t SetContainsValue( const Set* set ,void* value )
{
    if( set == NULL)
        return 0;

    return search_rbtree(set->tree, value) != NULL;
}

BOOLEAN_RETURN uint8_t SetIsEmpty( const Set* set  )
{
    return set->_size == 0;
}

BOOLEAN_RETURN uint8_t SetClear( Set* set  )
{
    if(clear_rbtree(set->tree))
    {
        set->_size = 0;
        
        
    }
    
    return set->_size == 0;
}

BOOLEAN_RETURN uint8_t SetRemoveValue( Set* set ,void* value )
{
    if( set == NULL)
        return 0;

    if(rb_tree_delete(set->tree, value) != NULL)
    {
        set->_size--;
        
        return 1;
    }
    
    return 0;
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    SetIteratorCallback + SetIterateValues
 */

struct SetContext
{
    const Set* self;
    void* context;
    SetIteratorCallback iter;
};

/*
static int Internal_SetIterateValues( void* data , void* user  )
{
    DEBUG_ASSERT(user);

    const struct SetContext* context = user;

    if( context == NULL)
        return 0;
    
    return context->iter( context->self , data , context->context ) == 1 ? 0 : 1;
}
*/
BOOLEAN_RETURN uint8_t SetIterateValues( const Set* set , SetIteratorCallback function , void *context  )
{
    UNUSED_PARAMETER(context);
    UNUSED_PARAMETER(function);
    if( set == NULL)
        return 0;
    /*
    
    struct SetContext ctx;
    ctx.self  = set;
    ctx.context = context;
    ctx.iter = function;
    
    GBIndex i = 0;
    void* dat =NULL;// RBIterFirst(iter, &set->tree);
    struct RBIter iter;
    for( RBIterInit(&iter) ,dat = RBIterFirst(&iter, &set->tree) ; dat != NULL ; dat = RBIterNext(&iter)  )
    {
        function(set , dat , context);
        i++;
    }
    
    return  i == SetGetSize(set);
    */
    return 0;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

SetIterator* SetCreateIterator( const Set* set)
{
    return new_tree_iterator(set->tree);
}

BOOLEAN_RETURN uint8_t SetIteratorHasNext( SetIterator* it)
{
    return tree_iterator_has_next( it );
}
void* SetIteratorGetNext( SetIterator* it )
{
    return tree_iterator_next( it);
}
void SetIteratorFree(SetIterator* it)
{
    destroy_iterator( it);
}

