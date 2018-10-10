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
//  GBFDSource.h
//  GroundBase
//
//  Created by Manuel Deneu on 12/11/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//


/**
 * \file GBFDSource.h
 * \brief C object that wrap a file descriptor source. See GBFDSource.hpp for C++ API.
 */


#ifndef GBFDSource_h
#define GBFDSource_h



#include <GBObject.h>
#include <GBRunLoopSource.h>


GB_BEGIN_DCL

#define UNINITIALIZED_FD   (int) -1

extern GBObjectClassRef GBFDSourceClass;
#define GBFDSourceClassName (const char*) "GBFDSource"

    
/*!
 * @discussion An opaque type wrapping a file descriptor provided in it's constructor ( GBFDSourceInitWithFD ).
 */
typedef struct _AbstractFileDescriptorSource GBFDSource;

/*!
 * @discussion Creates a new GBFDSource instance. GBFDSource is provided as a simple way to add externals (from GroundBase point of view) file descriptors to a GBRunLoop instance.
 * @param fd an opened & ready to use user file descriptor.
 * @param callback the nofication callback to use. Must be valid is attached later to a runloop. If used for send operations only can be NULL.
 * @return A fully initialized instance of GBFDSource.
 
 Warning : By default you must close the file descriptor yourself. See `GBFDSourceShouldCloseOnDestruct` method to let GBFDSource's destructor close the descriptor for you 
 */
GBFDSource* GBFDSourceInitWithFD( int fd , GBRunLoopSourceCallback callback );

/*!
 * @discussion Returns the underlying file descriptor number.
 * @param source a valid GBFDSource instance.
 * @return The file descriptor number on sucess, -1 on error.
 */
int GBFDSourceGetFileDescriptor( const GBFDSource* source);

// default is NO
void GBFDSourceShouldCloseOnDestruct( GBFDSource* source  , uint8_t shouldClose);

/*!
 * @discussion Attempts to read size bytes of data from the source. See man 2 read for more general informations about file descriptor reads.
 * @param source a valid GBFDSource instance.
 * @param content a valid address to copy the read buffer to. Will return GBSizeInvalid if NULL, undefined behaviour if invalid.
 * @param size the number of bytes to read. The effective read operation might read less bytes.
 * @return If successful, the number of bytes actually read, GBSizeInvalid otherwise.
 */
GBSize GBFDSourceRead( GBFDSource* source , void* content , GBSize size) GB_WARN_UNUSED_RESULT GB_NO_NULL_POINTERS;
GBSize GBFDSourceSend( GBFDSource* source , const void* data , GBSize dataLength ,int flags) GB_NO_NULL_POINTERS;
GBSize GBFDSourceWrite( GBFDSource* source , const void* data , GBSize dataLength ) GB_NO_NULL_POINTERS;

GB_END_DCL

#endif /* GBFDSource_h */
