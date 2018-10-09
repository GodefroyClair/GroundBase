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

/**
 * \file GBContainer.h
 * \brief GBContainer is an abstract type representing objects that store values.
 * Subclasses are:
 * GBList
 * GBArray
 * GBSet
 */

#ifndef GBContainer_h
#define GBContainer_h

#ifdef __cplusplus
extern "C" {
#endif

#include <GBTypes.h>
#include <GBObject.h>


typedef GBObject GBContainer;

// will work only with GBArrayClass GBListClass GBSetClass references
GBContainer* GBContainerInitWithType(  GBObjectClassRef classType);

BOOLEAN_RETURN uint8_t isGBContainter( GBRef object);

GBSize GBContainerGetSize( const GBContainer* container);

BOOLEAN_RETURN uint8_t GBContainerAddValue( GBContainer* container , GBRef value);

BOOLEAN_RETURN uint8_t GBContainerContainsValue( const GBContainer* container , GBRef value);

BOOLEAN_RETURN uint8_t GBContainerRemoveValue( GBContainer* container , GBRef value);

    
/*
 Callback signature for iterating over GBContainer values.
 Returns 1 to continue, 0 to stop.
 */
typedef int (*GBContainerIterator)(const GBContainer* container, GBRef value , void* context);

// Has no effects if container or method is NULL.
// Context will be passed to each call to GBContainerIterator. can be NULL.
void GBContainerIterate(const GBContainer* container , GBContainerIterator method , void* context);

#ifdef __cplusplus
}
#endif
#endif /* GBContainer_h */
