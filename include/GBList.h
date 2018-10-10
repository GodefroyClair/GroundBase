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
//  GBList.h
//  GroundBase
//
//  Created by Manuel Deneu on 28/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//


/**
 * \file GBList.h
 * \brief  GBList is a specialization of Set for GBObjects. It conforms to GBContainer protocol.
 */

#ifndef GBList_h
#define GBList_h




#include <GBObject.h>
#include <GBContainer.h>
#include <GBCommons.h>

GB_BEGIN_DCL

extern GBObjectClassRef GBListClass;
#define GBListClassName (const char*) "GBList"

/*!
 * @discussion An opaque GBList instance. This Array storage conforms to GBCollectionType. see GBCollectionType documentation.
 */
typedef struct _GBList GBList;

/*!
 * @discussion Initialize an empty GBList instance. You own the returned object. See GBObject ownership notes.
 * @return an empty GBList instance
 */
GBList* GBListInit(void);

// manage collection
GBSize GBListGetSize(const GBList* list);
void GBListReleaseContent(const GBList* list);

// access content
BOOLEAN_RETURN uint8_t GBListContainsValue(const GBList* list , GBRef value);
GB_DEPRECATED("5.3.6") GBRef GBListGetValueAtIndex(const GBList* list , GBIndex index);
GB_DEPRECATED("5.3.6") GBIndex GBListGetIndexForValue(const GBList* list , GBRef value);

// edit content
BOOLEAN_RETURN uint8_t GBListClear( GBList* list);
BOOLEAN_RETURN uint8_t GBListAddValue(GBList* list , GBRef value);
GB_DEPRECATED("5.3.6") BOOLEAN_RETURN uint8_t GBListRemoveValueAtIndex( GBList* list , GBIndex index);
BOOLEAN_RETURN uint8_t GBListRemoveValue( GBList* list , GBRef value);


void GBListIterate(const GBList* list , GBContainerIterator method , void* context);

typedef struct Node GBListIterator;

GBListIterator* GBListBegin( const GBList* list);
GBListIterator* GBListGetNext( const GBListIterator* iter);
GBRef GBListGetValue( const GBListIterator* iter);

#define GBListForEach( list , value) \
for(  GBListIterator* COMBINE(__iter , __LINE__) = GBListBegin(list) ;\
COMBINE(__iter , __LINE__) != NULL && ( value = GBListGetValue( COMBINE(__iter , __LINE__) ));\
COMBINE(__iter , __LINE__) = GBListGetNext( COMBINE(__iter , __LINE__) ) )

GB_END_DCL

#endif /* GBList_h */
