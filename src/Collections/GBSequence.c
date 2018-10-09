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
//  GBSequenceType.c
//  GroundBase
//
//  Created by Manuel Deneu on 15/10/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#include <string.h> // memcpy
#include <GBSequence.h>
#include <GBDictionary.h>
#include "GBSequence_Private.h"

BOOLEAN_RETURN uint8_t GBSequenceBaseInit( GBSequenceBase*self, GBSequenceCallbacks methods )
{
    DEBUG_ASSERT(self);
    if( self)
    {
        memcpy( &self->_methods , &methods , sizeof(GBSequenceCallbacks));
        //self->_impl = dataPtr;
        return 1;
    }
    return 0;
    
    //DEBUG_ASSERT(methods);
    
    return 0;
}

BOOLEAN_RETURN uint8_t isGBSequence( GBRef object)
{
    /*
    const GBSequenceBase* self = (const GBSequenceBase* ) object;
    
    return GBObjectIsValid(self) && self->_methods._addValueForKey != NULL && self->_methods._getSize != NULL;
     */
    
    return IsKindOfClass(object, GBDictionaryClass);
}

GBSize GBSequenceGetSize( const GBSequence* sequence)
{
    const GBSequenceBase* self = (const GBSequenceBase* ) sequence;
    return self->_methods._getSize(sequence);
    
}
BOOLEAN_RETURN uint8_t GBSequenceAddValueForKey(GBSequence* sequence , GBRef value , const GBString* key)
{
    GBSequenceBase* self = (GBSequenceBase* ) sequence;
    return self->_methods._addValueForKey(sequence,value , key);
}

BOOLEAN_RETURN uint8_t GSequenceContainsKey( const GBSequence* sequence , const GBString* value)
{
    const GBSequenceBase* self = (const GBSequenceBase* ) sequence;
    return self->_methods._containsKey(sequence , value);
}


GBRef GBSequenceGetValueForKey(const GBSequence* sequence , const GBString* key)
{
    const GBSequenceBase* self = (const GBSequenceBase* ) sequence;
    return self->_methods._getValue(sequence , key);
}


void GBSequenceIterateValues( const GBSequence* sequence , GBSequenceIterator method , void*context)
{
    if( sequence && method)
    {
        const GBSequenceBase* self = (const GBSequenceBase* ) sequence;
        self->_methods._iterate( sequence , method , context);
    }
}


static int _GBSequenceIterateForKeys(const GBSequence* sequence, const GBString* key , GBRef value , void* context)
{
    UNUSED_PARAMETER(sequence);
    UNUSED_PARAMETER(value);
    
    GBArray* list = context;
    DEBUG_ASSERT(context);
    if( context)
    {
        GBArrayAddValue(list, key);
    }
    
    return 1;

}

GBArray* GBSequenceGetKeys( const GBSequence* sequence)
{
    if( sequence)
    {
        GBArray* array = GBArrayInit();
        
        GBSequenceIterateValues(sequence, _GBSequenceIterateForKeys, array);
        return array;
    }
    return NULL;
}
