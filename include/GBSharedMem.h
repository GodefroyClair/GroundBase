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


/**
 * \file GBSharedMem.h
 * \brief SharedMemory read/write C API. See GBSharedMemory.hpp for C++ API.
 */


#ifndef GBSharedMem_h
#define GBSharedMem_h




#include <GBObject.h>
#include <GBString.h>


GB_BEGIN_DCL

extern GBObjectClassRef GBSharedMemClass;
#define GBSharedMemClassName (const char*) "GBSharedMem"

/*!
 * @discussion An opaque GBSharedMem instance.
 */
typedef struct _GBSharedMem GBSharedMem;

/*
 returns NULL if:
 - identifier is NULL, or invalid ( check ftok manpage for invalid values)
 - memSize == 0 or memSize == GBSizeInvalid
 - Will return NULL if the shared memory segment can't be created.
 */
GBSharedMem* GBSharedMemInitWriter( const GBString* identifier , GBSize memSize);
GBSharedMem* GBSharedMemInitReader( const GBString* identifier );


BOOLEAN_RETURN uint8_t GBSharedMemIsValid( const GBSharedMem* sharedMem);
const void* GBSharedMemGetReadHandle( const GBSharedMem* sharedMem);

const GBString* GBSharedMemGetIdentifier( const GBSharedMem* sharedMem);
GBSize GBSharedMemGetSize( const GBSharedMem* sharedMem);

/*
 will fail if
    sharedMem == NULL)
    data == NULL)
    size == 0 || size == GBSizeInvalid || size > sharedMem's size
 
 */
BOOLEAN_RETURN uint8_t GBSharedMemWrite( GBSharedMem* sharedMem , const void* data , GBSize size);

void* GBSharedMemGetWriteHandle( GBSharedMem* sharedMem);

GB_END_DCL

#endif /* GBSharedMem_h */
