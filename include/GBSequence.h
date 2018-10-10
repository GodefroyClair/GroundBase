/*
 * Copyright (c) 2016-2017 FlyLab
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
//  GBSequence.h
//  GroundBase
//
//  Created by Manuel Deneu on 15/10/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//


/**
 * \file GBSequence.h
 * \brief GBSequence is an abstract type representing objects that implement key-value access.
 */

#ifndef GBSequenceType_h
#define GBSequenceType_h

#include <GBObject.h>
#include <GBString.h>

GB_BEGIN_DCL

/*!
 * @typedef GBSequenceType
 * @brief Defines an opaque type for a GBObject instance conforming to GBSequenceType
 */
typedef GBObject GBSequence;

BOOLEAN_RETURN uint8_t isGBSequence( GBRef object);

GBSize GBSequenceGetSize( const GBSequence* sequence);
BOOLEAN_RETURN uint8_t GBSequenceAddValueForKey(GBSequence* sequence , GBRef value , const GBString* key);
BOOLEAN_RETURN uint8_t GSequenceContainsKey( const GBSequence* sequence , const GBString* key);

GBRef GBSequenceGetValueForKey(const GBSequence* sequence , const GBString* key);

/*
 Callback signature for iterating over GBSequence values.
 Returns 1 to continue, 0 to stop.
 */
typedef int (*GBSequenceIterator)(const GBSequence* sequence, const GBString* key , GBRef value , void* context);

// Has no effects if sequence or method is NULL.
// Context will be passed to each call to GBSequenceIterator. can be NULL.
void GBSequenceIterateValues( const GBSequence* sequence , GBSequenceIterator method , void*context);

// needs release!
GBArray* GBSequenceGetKeys( const GBSequence* sequence);

GB_END_DCL

#endif /* GBSequenceType_h */
