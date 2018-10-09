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

#include <string.h> // memcpy
#include <GBContainer.h>
#include "../GBObject_Private.h"
#include "GBContainer_Private.h"


#include <GBSet.h>
#include <GBArray.h>
#include <GBList.h>


BOOLEAN_RETURN uint8_t GBContainerBaseInit(GBContainerBase*self, GBContainerCallbacks methods)
{
    DEBUG_ASSERT(self);
    
    if( self)
    {
        memcpy( &self->_methods , &methods , sizeof(GBContainerCallbacks));
        //self->_impl = dataPtr;
        return 1;
    }
    return 0;
}


GBContainer* GBContainerInitWithType(  GBObjectClassRef classType)
{
    if( classType == GBArrayClass)
    {
        return GBArrayInit();
    }
    else if( classType == GBListClass)
    {
        return GBListInit();
    }
    else if( classType == GBSetClass)
    {
        return GBSetInit();
    }
    
    return NULL;
}

BOOLEAN_RETURN uint8_t isGBContainter( GBRef object)
{
    
    return IsKindOfClass(object, GBArrayClass) || IsKindOfClass(object, GBListClass) || IsKindOfClass(object, GBSetClass);
    /*
    const GBContainerBase* self = (const GBContainerBase* ) object;
    
    return GBObjectIsValid(self) && self->_methods._addValue != NULL && self->_methods._getSize != NULL;
     */
}

GBSize GBContainerGetSize( const GBContainer* container)
{
    const GBContainerBase* self = (const GBContainerBase* ) container;
    return self->_methods._getSize(container);
}

BOOLEAN_RETURN uint8_t GBContainerAddValue( GBContainer* container , GBRef value)
{
    GBContainerBase* self = ( GBContainerBase* ) container;
    return self->_methods._addValue(container , value);
}

BOOLEAN_RETURN uint8_t GBContainerContainsValue( const GBContainer* container , GBRef value)
{
    const GBContainerBase* self = (const GBContainerBase* ) container;
    return self->_methods._containsValue(container , value);
}

BOOLEAN_RETURN uint8_t GBContainerRemoveValue( GBContainer* container , GBRef value)
{
    GBContainerBase* self = ( GBContainerBase* ) container;
    return self->_methods._removeValue(container , value);
}

void GBContainerIterate(const GBContainer* container , GBContainerIterator method , void* context)
{
    if( container && method)
    {
        GBContainerBase* self = ( GBContainerBase* ) container;
        
        self->_methods._iterate(container , method , context);
    }
}

