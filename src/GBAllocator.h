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


#ifndef GBAllocator_h
#define GBAllocator_h

#include <GBTypes.h>
#include <GBCommons.h>


typedef struct
{
    void* (* Malloc ) (GBSize size , const void* self);
    void* (* Realloc) (void* ptr, GBSize size , const void* self);
    void* (* Calloc ) (GBSize count, GBSize size , const void* self);
    void  (* Free   ) (void* ptr , const void* self);
    void* usrPtr;
    
} GBAllocator;

extern const GBAllocator GBDefaultAllocator;

// OLD Name, use GBDefaultAllocator instead. Will be removed at some point.
#define DefaultAllocator GBDefaultAllocator


/*
 -  Default is standard malloc/realloc,calloc,free.
 -  Can be redefined for garbage collection or debugging.
 */
void* GB_MALLOC_LIKE GBMalloc(GBSize size);
void* GB_MALLOC_LIKE GBRealloc(void *ptr, GBSize size);
void* GB_MALLOC_LIKE GBCalloc( GBSize count, GBSize size);

void  GBFree( void* ptr);

GBSize GBAllocatorGetTotalAllocatedCount(void);
GBSize GBAllocatorGetTotalFreedCount(void);

#endif /* GBAllocator_h */
