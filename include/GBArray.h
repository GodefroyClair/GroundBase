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
//  GBArray.h
//  GroundBase
//
//  Created by Manuel Deneu on 31/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//


/**
 * \file GBArray.h
 * \brief  C GBObject's Array . See GBArray.hpp for C API
 */


#ifndef GBArray_h
#define GBArray_h



#include <GBCommons.h>
#include <GBObject.h>
#include <GBContainer.h>

GB_BEGIN_DCL

extern GBObjectClassRef GBArrayClass;
#define GBArrayClassName (const char*) "GBArray"


/*!
 * @discussion An opaque type for an array. GBArray conforms to GBCollectionType. see GBCollectionType documentation
 */
typedef struct _GBArray GBArray;

/*!
 * @discussion Initialize an empty GBArray instance. You own the returned object. See GBObject ownership notes.
 * @return an empty GBArray instance
 */
GBArray* GBArrayInit(void);

/*!
 * @discussion Initialize a GBArray instance with a given capacity. You own the returned object. See GBObject ownership notes.
 * @return an empty GBArray instance
 */
GBArray* GBArrayInitWithCapacity(GBSize cap);

BOOLEAN_RETURN uint8_t GBArraySetCapacity( GBArray* array , GBSize newCapacity);

GBSize GBArrayGetCapacity(const GBArray* array);

BOOLEAN_RETURN uint8_t GBArraySetValueAt(GBArray *array, GBIndex pos, const void* val);

// manage collection
GBSize GBArrayGetSize(const GBArray *array);
void GBArrayReleaseContent(const GBArray *array);

// access content
BOOLEAN_RETURN uint8_t GBArrayContainsValue( const GBArray *array, const void* val);
GBRef GBArrayGetValueAtIndex( const GBArray *array, GBIndex pos);
GBIndex GBArrayGetIndexForValue(const GBArray* array , GBRef value);



// edit content
BOOLEAN_RETURN uint8_t GBArrayClear( GBArray* array );
BOOLEAN_RETURN uint8_t GBArrayAddValue( GBArray* array , GBRef value);
BOOLEAN_RETURN uint8_t GBArrayRemoveValueAtIndex( GBArray* array , GBIndex index);
BOOLEAN_RETURN uint8_t GBArrayRemoveValue( GBArray* array , GBRef value);

void GBArrayIterate(const GBArray* array , GBContainerIterator method , void* context);

#define GBArrayForEach(array , value) \
for (GBIndex COMBINE(__iter , __LINE__) = 0;\
(COMBINE(__iter , __LINE__)<GBArrayGetSize(array) ) && ( value = GBArrayGetValueAtIndex(array, COMBINE(__iter , __LINE__)));\
COMBINE(__iter , __LINE__)++)

GB_END_DCL

#endif /* GBArray_h */
