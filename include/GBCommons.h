/*
 * Copyright (c) 2016-217 FlyLab
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
//  GBCommons.h
//  GroundBase
//
//  Created by Manuel Deneu on 28/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//


/**
* \file GBCommons.h
* \brief Various Macros for GroundBase.
*/

#ifndef GroundBase_Commons_h
#define GroundBase_Commons_h

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__APPLE__) && defined(__MACH__)
//#include <sys/socket.h> // SO_NOSIGPIPE
#endif


#define GB_PURE                __attribute__((pure))
#define GB_HOT                __attribute__((hot))
#define GB_COLD                __attribute__((cold))
#define GB_MALLOC_LIKE         __attribute__((malloc))
#define GB_NO_NULL_POINTERS    __attribute__((nonnull))
#define GB_NO_NULL_ARGS(index, range)    __attribute__((nonnull(index, range) ))
#define GB_WARN_UNUSED_RESULT  __attribute__((warn_unused_result))

#define GB_SCANF_LIKE(index ,range) __attribute__ ((format (scanf, index, range)))

#ifdef DEBUG
#include <stdio.h> //perror printf
#endif
// This macro marks Methods that return boolean value instead of UNIX return code, ie with `BOOLEAN_RETURN` success is 1 instead of 0.
#define BOOLEAN_RETURN

/* This macro marks suspicious const ptr to ptr casts that should be removed/avoided in the future */
#define CONST_CAST( Type) (Type)


/* Do not put any vital code into DEBUG_ASSERT since it will turn into a no-op on release build. */
#ifdef DEBUG
#define DEBUG_ASSERT(x) assert(x)
#else
#define DEBUG_ASSERT(x)
#endif

/* just a printf that disapears on release */
#ifdef DEBUG
#define DEBUG_LOG( ...) printf( __VA_ARGS__)
#else
#define DEBUG_LOG( ...)
#endif

/* just a print on stderr that disapears on release */
#ifdef DEBUG
#define DEBUG_ERR( ...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG_ERR( ...)
#endif

/* just a perror that disapears on release */
#if DEBUG
#define PERROR(...) perror(__VA_ARGS__)
#else
#define PERROR(...)
#endif


#define GB_ALWAYS_INLINE static inline

/** Use this macro on unused parameters to silence compiler warning Complains.
 *  \def UNUSED_PARAMETER
 */
#ifndef UNUSED_PARAMETER
#define UNUSED_PARAMETER(x) (void)(x)
#endif

/* Used by GBSequenceForEach */
#define  COMBINE__1(X,Y) X##Y
#define COMBINE(X,Y) COMBINE__1(X,Y)

#define GB_DEPRECATED(version) __attribute((deprecated))



#ifdef __cplusplus
#define GB_BEGIN_DCL extern "C" {
#else
#define GB_BEGIN_DCL
#endif



#ifdef __cplusplus
#define GB_END_DCL }
#else
#define GB_END_DCL
#endif

#endif /* GroundBase_Commons_h */
