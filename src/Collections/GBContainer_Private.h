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

#ifndef GBContainer_Private_h
#define GBContainer_Private_h

#include "../GBObject_Private.h"
#include <GBContainer.h>


typedef enum
{
    ContainerUnknown  = 0,
    ContainerSet      = 1,
}ContainerImplType;



typedef GBSize (*_GBContainerGetSize)(const GBContainer* container);
typedef BOOLEAN_RETURN uint8_t (*_GBContainerAddValue)(GBContainer* container , GBRef value);
typedef BOOLEAN_RETURN uint8_t (*_GBContainerRemoveValue)(GBContainer* container , GBRef value);
typedef BOOLEAN_RETURN uint8_t (*_GBContainerContainsValue)( const GBContainer* container , GBRef value);

typedef void (*_GBContainerIterate)(const GBContainer* list , GBContainerIterator method , void* context);
//typedef BOOLEAN_RETURN uint8_t (*_GBContainerReleaseContent)(GBContainer* container );


typedef struct
{
    _GBContainerGetSize          _getSize;
    _GBContainerAddValue         _addValue;
    _GBContainerRemoveValue      _removeValue;
    _GBContainerContainsValue    _containsValue;
    _GBContainerIterate  _iterate;
    
    
}GBContainerCallbacks;


typedef struct
{
    GBObjectBase base;
    GBContainerCallbacks _methods;
    //void* _impl;
    
} GBContainerBase;


BOOLEAN_RETURN uint8_t GBContainerBaseInit(GBContainerBase*self, GBContainerCallbacks methods );

#endif /* GBContainer_Private_h */
