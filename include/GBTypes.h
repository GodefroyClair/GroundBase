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
//  GBTypes.h
//  GroundBase
//
//  Created by Manuel Deneu on 04/09/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#ifndef GBTypes_h
#define GBTypes_h

#include <stddef.h>
#include <stdint.h>
#include <GBCommons.h>

GB_BEGIN_DCL

/**
 * \file GBTypes.h
 * \brief GroundBase types.
 */

/**
 * \typedef GBObject
 * \brief Defines an opaque type for a GBObject instance
 */
typedef void GBObject;


/*!
 * @typedef GBRef
 * @brief Defines an opaque handle for a GBObject instance
 */
typedef const GBObject* GBRef;


/*!
 * @typedef GBHashCode
 * @brief Defines a hash code returned by GBHash
 */
typedef uint32_t GBHashCode;

/*!
 * @typedef GBCollectionType
 * @brief Defines an opaque type for a GBObject instance conforming to GBCollectionType
 */
//typedef void GBCollectionType;



/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/* TODO :The following definitions are commons with other GroundBase implementations. 
 For now they are copied and shipped with other implementations. 
 A common header should be added.
 */

/*!
 * @typedef GBIndex
 * @brief Defines an unsigned integer type used in GroundBase as a collection index.
 */
typedef size_t GBIndex;

/*!
 * @typedef GBSize
 * @brief Defines an unsigned  integer type used in GroundBase as a collection size.
 */
typedef size_t GBSize;

/**
 * \var GBIndexInvalid
 * \brief Special GBIndex value for invalid indexes.
 */
static const GBIndex GBIndexInvalid = SIZE_MAX;

/**
 * \var GBSizeInvalid
 * \brief Special GBSize value for invalid size.
 */
static const GBIndex GBSizeInvalid = SIZE_MAX;

/*!
 * @typedef GBTimeMS
 * @brief Defines an integer type used in GroundBase as millisecond time.
 */
typedef uint64_t GBTimeMS;

/**
 * \var GBTimeMSInvalid
 * \brief Special GBSGBTimeMSize value for invalid time.
 */
static const GBTimeMS GBTimeMSInvalid = UINT64_MAX;

GB_END_DCL

#endif /* GBTypes_h */
