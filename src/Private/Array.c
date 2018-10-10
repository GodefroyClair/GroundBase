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
//  Array.c
//  GroundBase
//
//  Created by Manuel Deneu on 31/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//


#include <string.h> // memmove
#include "Array.h"
#include <GBAllocator.h>

#define ARRAY_MIN_CAPACITY (GBSize) 10

struct _Array
{
    const  void**  data;     /* pointer to the data array */
    
    GBSize size;       /* Number of elements in the array */
    GBSize capacity;   /* capacity ofthe array */
};

Array* ArrayInit()
{
    Array *r = (Array *) GBMalloc(sizeof( Array));
    r->capacity = 0;
    r->size = 0;
    r->data = NULL;
    return r;
}

Array* ArrayInitWithCapacity(GBSize cap)
{

    Array *r = ArrayInit();

    if( r && cap > 0)
    {
        ArraySetCapacity(r, cap);
    }
    return r;
}



void ArrayFree(Array *array )
{
    if( array)
    {
        if(array->data)
        {
            GBFree(array->data);
        }
        
        GBFree(array);
    }

    
}

BOOLEAN_RETURN uint8_t ArraySetCapacity( Array* array , GBSize newCapacity)
{
    if( array == NULL)
        return 0;
    
    if( newCapacity <= array->capacity)
        return 1;
    
    const void** newData = GBRealloc(array->data, newCapacity*sizeof(const void*) );
    
    
    
    if( newData )
    {
        const size_t oldSize = array->capacity;
        array->data =newData;
        array->capacity = newCapacity;
        
        for (int i = 0; i < (int) (array->capacity - oldSize); i++)
        {
            array->data[(int)oldSize+i] = NULL;
            
        }
        return 1;
    }

    return 0;
}



GBSize ArrayGetSize(const Array *array)
{
    if( array == NULL)
        return 0;
    
    return array->size;
}

GBSize ArrayGetCapacity(const Array* array)
{
    if( array == NULL)
        return 0;
    
    return array->capacity;
}

BOOLEAN_RETURN uint8_t ArrayAddValue(Array *array, const void* val)
{
    if( array == NULL)
        return 0;

    if(array->size == array->capacity)
    {
        GBSize newCap = array->capacity == 0? ARRAY_MIN_CAPACITY : array->capacity*2;
        
        if(ArraySetCapacity(array, newCap ) == 0)
            return 0;
    }
    
    array->data[array->size++] = val;

    return 1;
    
}

BOOLEAN_RETURN uint8_t ArraySetValueAt(Array *array, GBIndex pos, const void* val)
{
    if (array == NULL)
        return 0;
    
    if( pos >= array->size )
        return 0;
    
    array->data[pos] = val;
    
    return 1;
}



BOOLEAN_RETURN uint8_t ArrayReplaceValueAtIndex( Array* array  , GBIndex index , const void* newValue)
{
    if(array == NULL)
        return 0;
    
    if( array->data == NULL)
        return 0;
    
    if( index >= array->size )
        return 0;
    
    array->data[ index ] = newValue;
    
    
    return 1;
}

const void* ArrayGetValueAtIndex( const Array *array, GBIndex pos)
{
    if(array == NULL)
        return NULL;
    
    if( array->data == NULL)
        return NULL;
    
    if( pos >= array->size )
        return NULL;
    
    const void* dat = array->data[pos];
    
    return dat;
}

GBIndex ArrayGetIndexForValue(const Array* array , const void* value)
{
    
    if( array == NULL)
        return GBIndexInvalid;
    
    for (size_t i = 0 ; i< ArrayGetSize(array);i++)
    {
        if( ArrayGetValueAtIndex(array, i) == value  )
            return i;
    }
    
    return GBIndexInvalid;
}

BOOLEAN_RETURN uint8_t ArrayContainsValue( const Array *array, const void* val)
{    
    return ArrayGetIndexForValue(array , val) != GBIndexInvalid;
}

BOOLEAN_RETURN uint8_t ArrayRemoveValueAtIndex( Array* array , GBIndex pos)
{
    if( array == NULL)
        return 0;
    
    if( pos >= array->size)
        return 0;
    
    if( array->size >(size_t) pos)
    {
        for (GBIndex i = pos+1 ; i< array->size;i++)
        {
            array->data[i-1] = array->data[i];
        }
        array->size--;
    }
    return 1;
}

BOOLEAN_RETURN uint8_t ArrayRemoveValues( Array* array , GBIndex indexStart , GBSize numElements)
{
    if( array == NULL)
        return 0;
    
    if( indexStart >= array->size)
        return 0;
    
    if( indexStart + numElements > array->size)
        return 0;
    
    
    const GBIndex startToMove = indexStart + numElements;
    
    const GBSize numToMove = array->size -  startToMove;
    
    void * dest = array->data+indexStart;
    const void* source = array->data+startToMove;
    
    const void* ret = memmove( dest, source, numToMove);
    
    if( ret == dest )
    {
        array->size -= (size_t) numElements;
        return 1;
    }
    
    return 0;
}
BOOLEAN_RETURN uint8_t ArrayClear( Array* array)
{
    return ArrayRemoveValues(array, 0,  ArrayGetSize(array));
}

