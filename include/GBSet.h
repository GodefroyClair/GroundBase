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
 * \file GBSet.h
 * \brief A Containter that cna store GBObject instances, without any particular order, and no repeated values.
 */

#ifndef GBSet_h
#define GBSet_h



#include <GBContainer.h>

GB_BEGIN_DCL
    
/*!
 * @discussion An opaque type for a set.
 */
typedef struct _GBSet GBSet;

extern GBObjectClassRef GBSetClass;
#define GBSetClassName (const char*) "GBSet"

GBSet* GBSetInit(void);

GBSize GBSetGetSize( const GBSet* set);

BOOLEAN_RETURN uint8_t GBSetAddValue( GBSet* set , GBRef value);
BOOLEAN_RETURN uint8_t GBSetContainsValue(const GBSet* set , GBRef value);
BOOLEAN_RETURN uint8_t GBSetRemoveValue( GBSet* set , GBRef value);

BOOLEAN_RETURN uint8_t GBSetClear( GBSet* set);

void GBSetIterate(const GBSet* set , GBContainerIterator method , void* context);

/* **** */
/*
typedef struct siterator GBSetIterator;

GBSetIterator* GBSetCreateIterator(const  GBSet* set);
BOOLEAN_RETURN uint8_t GBSetIteratorHasNext( GBSetIterator* it);
GBRef GBSetIteratorGetNext( GBSetIterator* it );
void GBSetIteratorFree(GBSetIterator* it);
*/
    
GB_END_DCL

#endif /* GBSet_h */
