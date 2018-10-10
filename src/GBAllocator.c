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


#include <stdlib.h>
#include <GBCommons.h>
#include "GBAllocator.h"



static GBSize _allocNum = 0;
static GBSize _freedNum = 0;

static void* DefMalloc(GBSize size , const void *self);
static void* DefRealloc(void *ptr, GBSize size , const void *self);
static void* DefCalloc( GBSize count, GBSize size , const void *self);
static void  DefFree( void* ptr , const void *self);

static void* StackMalloc(GBSize size , const void *self);
static void* StackRealloc(void *ptr, GBSize size , const void *self);
static void* StackCalloc( GBSize count, GBSize size , const void *self);
static void  StackFree( void* ptr , const void *self);

const GBAllocator GBDefaultAllocator =
{
    DefMalloc,
    DefRealloc,
    DefCalloc,
    DefFree,
    NULL
};






static void* DefMalloc(GBSize size , const void *self)
{
    UNUSED_PARAMETER(self);
    return GBMalloc(size );
}

static void* DefRealloc(void *ptr, GBSize size , const void *self)
{
    UNUSED_PARAMETER(self);
    return GBRealloc(ptr, size);
}

static void* DefCalloc( GBSize count, GBSize size , const void *self)
{
    UNUSED_PARAMETER(self);
    return GBCalloc(count, size);
}

static void  DefFree( void* ptr , const void *self)
{
    UNUSED_PARAMETER(self);
    GBFree(ptr);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

const GBAllocator GBStackAllocator =
{
    StackMalloc,
    StackRealloc,
    StackCalloc,
    StackFree,
    NULL
};


static void* StackMalloc(GBSize size , const void *self)
{
    UNUSED_PARAMETER(self);
    return GBMalloc(size );
}

static void* StackRealloc(void *ptr, GBSize size , const void *self)
{
    UNUSED_PARAMETER(self);
    return GBRealloc(ptr, size);
}

static void* StackCalloc( GBSize count, GBSize size , const void *self)
{
    UNUSED_PARAMETER(self);
    return GBCalloc(count, size);
}

static void  StackFree( void* ptr , const void *self)
{
    UNUSED_PARAMETER(self);
    GBFree(ptr);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void* GBMalloc(GBSize size)
{
    _allocNum += 1;
    void* ptr = malloc(size);
    DEBUG_ASSERT(ptr);
    return ptr;
}

void* GBRealloc(void *ptr, GBSize size)
{
    return realloc(ptr, size);
}

void* GBCalloc( GBSize count, GBSize size)
{
    _allocNum += count;
    return calloc(count, size);
}

void  GBFree( void* ptr)
{

    _freedNum += 1;
    
    free(ptr);
}


GBSize GBAllocatorGetTotalAllocatedCount()
{
    return _allocNum;
}
GBSize GBAllocatorGetTotalFreedCount()
{
    return _freedNum;
}
