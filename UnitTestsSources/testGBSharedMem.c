//
//  testGBSharedMem.c
//  GroundBase
//
//  Created by Manuel Deneu on 23/12/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "testGBSharedMem.h"
#include <GroundBase.h>

// temp
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>

#include <GBString.h>
#include <GBSharedMem.h>

typedef struct
{
    uint32_t counter;
    char buff[256];
} DataBuf;

static const GBSize numIters = 100;


static void mainWriter()
{
    const GBString* id = GBStringInitWithCStr("UnitTests");
    GBSharedMem* writer = GBSharedMemInitWriter(id  , sizeof(DataBuf) );
    
    
    DataBuf* buffer =(DataBuf*)  GBSharedMemGetWriteHandle(writer);
    assert(buffer);
    
    for (GBIndex i= 0; i< numIters ;++i)
    {
        buffer->counter = (uint32_t) i;
        usleep(1000);
    }
    

    assert(writer);
    GBRelease(writer);
    GBRelease(id);
    
}

static void mainReader()
{
    usleep(1);
    
    printf("Started Child\n");
    const GBString* id = GBStringInitWithCStr("UnitTests");
    GBSharedMem* reader = GBSharedMemInitReader(id  );
    if( reader)
    {
        printf("Shared Mem writer Created !\n");
        assert(GBSharedMemGetSize(reader) == sizeof(DataBuf));
        
        const DataBuf* buffer =( const DataBuf*)  GBSharedMemGetReadHandle(reader);
        assert(buffer);
        
        for (GBIndex i= 0; i< numIters ;++i)
        {
            printf("Buffer counter %i\n" ,buffer->counter);
            
            usleep(1000);
        }

        GBRelease(reader);
    }
    else
    {
        printf("Unable to create GBSharedMem Reader for id '%s'\n" , GBStringGetCStr(id));
    }
    GBRelease(id);
    
    exit(0);
    
}




static void testInvalidSharedMems()
{
    assert( GBSharedMemInitReader(NULL) == NULL );
    assert( GBSharedMemInitWriter(NULL , 0) == NULL );
    assert( GBSharedMemInitWriter(NULL , GBSizeInvalid) == NULL );
    assert( GBSharedMemInitWriter(NULL , 100) == NULL );
    
    {
        const GBString* invalidButLegitID = GBStringInitWithCStr("éaà|azer");
        
        assert( GBSharedMemInitWriter( invalidButLegitID , 0) == NULL );
        assert( GBSharedMemInitWriter( invalidButLegitID , GBSizeInvalid) == NULL );
        assert( GBSharedMemInitWriter( invalidButLegitID , 100) == NULL );
        
        
        assert( GBSharedMemInitReader(invalidButLegitID) == NULL);
        
        GBRelease(invalidButLegitID);
    }
    {
        const GBString* legitID = GBStringInitWithCStr("UnitTests");
        assert( GBSharedMemInitWriter( legitID , 0) == NULL );
        
        assert( GBSharedMemInitReader(legitID) == NULL);
        GBRelease(legitID);
    }
}

void testGBSharedMem()
{
    testInvalidSharedMems();
    
    printf("----- test GBSharedMem ----- \n");
    
    pid_t t = fork();
    if( t == 0)
    {
        mainReader();
    }
    else
    {
        printf("Started parent\n ");
        mainWriter();
        
    }
    
    

}
