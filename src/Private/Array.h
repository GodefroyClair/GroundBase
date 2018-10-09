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
//  Array.h
//  GroundBase
//
//  Created by Manuel Deneu on 31/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

/*
  Array Class is for GroundBase's internal use only. Its main purpose if to serve as GBArray Backend.
 */

#ifndef Array_h
#define Array_h

#include <stdio.h>
#include <GBTypes.h>
#include <GBCommons.h>

typedef struct _Array Array;

Array* ArrayInit(void);
Array* ArrayInitWithCapacity(GBSize cap);
void ArrayFree(Array *array);

BOOLEAN_RETURN uint8_t ArraySetCapacity( Array* array , GBSize newCapacity);

BOOLEAN_RETURN uint8_t ArrayAddValue(Array *array, const void* val);

BOOLEAN_RETURN uint8_t ArrayContainsValue( const Array *array, const void* val);

GBSize ArrayGetSize(const Array *array);
GBSize ArrayGetCapacity(const Array* array);


BOOLEAN_RETURN uint8_t ArrayReplaceValueAtIndex( Array* array  , GBIndex index , const void* newValue);
const void* ArrayGetValueAtIndex( const Array *array, GBIndex pos);
GBSize ArrayGetIndexForValue(const Array* array , const void* value);

BOOLEAN_RETURN uint8_t ArrayRemoveValueAtIndex( Array* array , GBIndex pos);
BOOLEAN_RETURN uint8_t ArrayClear( Array* array);

BOOLEAN_RETURN uint8_t ArrayRemoveValues( Array* array , GBIndex indexStart , GBSize numElements);

BOOLEAN_RETURN uint8_t ArraySetValueAt(Array *array, GBIndex pos, const void* val);


#define ArrayForEach(array , value) \
for (GBIndex COMBINE(__iter , __LINE__) = 0;\
(COMBINE(__iter , __LINE__)< ArrayGetSize(array) ) && ( value = ArrayGetValueAtIndex(array, COMBINE(__iter , __LINE__)));\
COMBINE(__iter , __LINE__)++)

#endif /* Array_h */
