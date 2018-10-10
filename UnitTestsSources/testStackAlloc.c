//
//  testStackAlloc.c
//  UnitTests
//
//  Created by Manuel Deneu on 09/10/2018.
//  Copyright © 2018 Manuel Deneu. All rights reserved.
//

#include "testStackAlloc.h"

#include <GBAllocator.h>
#include <GBUPCService.h>
#include <assert.h>

#include <stdio.h>


static void* StackMalloc(GBSize size , const void *self);
static void* StackRealloc(void *ptr, GBSize size , const void *self);
static void* StackCalloc( GBSize count, GBSize size , const void *self);
static void  StackFree( void* ptr , const void *self);

const GBAllocator stackAllocator =
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



void testStackAlloc()
{
    printf("testStackAlloc\n");
    
    
    uint8_t service_buffer[GBUPCService_Size];
    
    const GBString* name = GBStringInitWithCStr("MyService");
    GBUPCService* service = GBUPCServiceInitWithName_OnStack(name, service_buffer);
    GBRelease(name);
    
    assert( IsKindOfClass(service, GBUPCServiceClass));
    assert(service);
    assert(GBObjectIsValid(service));
    
    GBRelease(service);
    
}
