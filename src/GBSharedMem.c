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

#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h> //memcpy
#include <errno.h>

#include <GBSharedMem.h>
#include "GBObject_Private.h"



static void * ctor(void * _self, va_list * app);
static void * dtor (void * _self);

static uint8_t  equals (const void * _self, const void * _b);
static GBRef description (const void * self);

/**/

BOOLEAN_RETURN uint8_t Internal_invalidate( GBSharedMem* self);

void* Internal_initializeSharedMemory( GBSharedMem *self , int flags);
BOOLEAN_RETURN uint8_t Internal_releaseSharedMemory( GBSharedMem* self);
/**/

/*
#ifndef _POSIX_THREAD_PROCESS_SHARED
#error "_POSIX_THREAD_PROCESS_SHARED not def!"
#endif
*/




struct _GBSharedMem
{
    GBObjectBase    base;
    const GBString* _identifier;
    int8_t          _isWriter; /* -1 : invalid , 0 : reader ,  1 : writter*/
    key_t           _associatedKey;
    int             _shmid;
    GBSize          _size;

    uint8_t*        _ptr;

};

static const GBObjectClass _GBSharedMemClass =
{
    sizeof(struct _GBSharedMem ),
    ctor,
    dtor,
    NULL,// NOT clonable
    equals,
    description,
    NULL, // initialize
    NULL, // deinit
    NULL, // retain
    NULL, // release
    (char*)GBSharedMemClassName
};

GBObjectClassRef GBSharedMemClass = & _GBSharedMemClass;

static void * ctor(void * _self, va_list * app)
{
    UNUSED_PARAMETER(app);
    GBSharedMem* self =(GBSharedMem*) _self;
    
    if( self)
    {
        Internal_invalidate(self);
        
        self->_isWriter = - 1;
        self->_identifier = NULL;

        
        return self;
    }
    
    return NULL;

}
static void * dtor (void * _self)
{
    GBSharedMem* self =(GBSharedMem*) _self;
    
    if( self)
    {
        if( self->_identifier)
        {
            GBRelease( self->_identifier);
        }
        
        

        
        Internal_releaseSharedMemory(self);
        
        
        return self;
    }
    return NULL;
}

static uint8_t  equals (const void * _self, const void * _b)
{
    UNUSED_PARAMETER(_self);
    UNUSED_PARAMETER(_b);
    return 0;
}
static GBRef description (const void * _self)
{
    const GBSharedMem* self =( const GBSharedMem*) _self;
    
    if (self)
    {
        
        return GBStringInitWithFormat("%s size %zi" , self->_isWriter? "Writter" : "Reader" , self->_size);
    }
    return NULL;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*
GBSharedMem* GBSharedMemInit()
{
    return GBObjectAlloc( GBDefaultAllocator,GBSharedMemClass );
}
 */

GBSharedMem* GBSharedMemInitWriter( const GBString* identifier , GBSize memSize)
{
    if( identifier == NULL || memSize == GBSizeInvalid || memSize == 0 )
        return NULL;
    
    
    key_t key = ftok(GBStringGetCStr(identifier), 0);
    if( key == -1)
    {
        PERROR("GBSharedMemInitWriter.ftok");
        return NULL;
    }

    GBSharedMem *self = GBObjectAlloc( GBDefaultAllocator,GBSharedMemClass );
    if( self)
    {
        self->_identifier = GBObjectClone(identifier);
        self->_associatedKey = key;
        
        
        self->_size = memSize ;
        
        self->_isWriter = 1;
        self->_ptr =  Internal_initializeSharedMemory( self ,IPC_CREAT | 0666);
        
        
        struct shmid_ds shm_ds;
        memset(&shm_ds, 0, sizeof(struct shmid_ds));
        if(shmctl(self->_shmid, IPC_STAT, &shm_ds) != 0)
        {
            perror("GBSharedMemInitWriter.shmctl\n");
        }
        assert(shm_ds.shm_segsz == memSize);

        
    
        return self;
    }
    
    return NULL;
}

GBSharedMem* GBSharedMemInitReader( const GBString* identifier )
{
    if( !identifier )
        return NULL;
    
    key_t key = ftok(GBStringGetCStr(identifier), 0);
    if( key == -1)
    {
        PERROR("GBSharedMemInitReader.ftok");
        return NULL;
    }
    
    
    GBSharedMem *self = GBObjectAlloc( GBDefaultAllocator,GBSharedMemClass );
    if( self)
    {

        self->_identifier = GBObjectClone(identifier);
        
        self->_associatedKey =  key;
        
        self->_size = 0;
        self->_isWriter = 0;
        self->_ptr = Internal_initializeSharedMemory(self , 0666 | SHM_RDONLY );
        
        if( self->_ptr == NULL)
        {
            GBRelease(self);
            return NULL;
        }

        struct shmid_ds shm_ds;
        if(shmctl(self->_shmid, IPC_STAT, &shm_ds) != 0)
        {
            PERROR("GBSharedMemInitReader.shmctl");
            GBRelease(self);
            return NULL;
        }
        
        self->_size = shm_ds.shm_segsz;
        return self;
    }
    return NULL;
}

BOOLEAN_RETURN uint8_t GBSharedMemIsValid( const GBSharedMem* sharedMem)
{
    if( sharedMem)
    {
        return sharedMem->_ptr != NULL;
    }
    
    return 0;
}

const void* GBSharedMemGetReadHandle( const GBSharedMem* sharedMem)
{
    if( sharedMem == NULL)
        return NULL;
    return sharedMem->_ptr;
}

void* GBSharedMemGetWriteHandle( GBSharedMem* sharedMem)
{

    if( sharedMem == NULL)
        return NULL;
    return sharedMem->_ptr;
}


BOOLEAN_RETURN uint8_t GBSharedMemWrite( GBSharedMem* sharedMem , const void* data , GBSize size)
{
    DEBUG_ASSERT( sharedMem->_isWriter == 1 );
    
    if( sharedMem == NULL)
        return 0;
    if( data == NULL)
        return 0;
    
    if( size == 0 || size == GBSizeInvalid || size > sharedMem->_size )
        return 0;
    
    
    return memcpy(sharedMem->_ptr, data, size) == sharedMem->_ptr;
}

const GBString* GBSharedMemGetIdentifier( const GBSharedMem* sharedMem)
{
    if( sharedMem)
    {
        return  sharedMem->_identifier;
    }
    
    return NULL;
}

GBSize GBSharedMemGetSize( const GBSharedMem* sharedMem)
{
    if( sharedMem)
    {
        return sharedMem->_size;
    }
    return GBSizeInvalid;
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    Internal methods
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

BOOLEAN_RETURN uint8_t Internal_invalidate( GBSharedMem* self)
{
    DEBUG_ASSERT( self);
    
    if( self)
    {
        self->_associatedKey = 0;
        self->_ptr    = NULL;
        self->_size   = GBSizeInvalid;
        self->_shmid  = -1;
        return 1;
    }
    
    return 0;
}
void* Internal_initializeSharedMemory( GBSharedMem *self , int flags)
{
    self->_shmid = -1;
    void *sharedPointer = NULL;
    
    DEBUG_ASSERT(   (self->_isWriter && self->_size > 0)
                 || (self->_isWriter == 0 && self->_size == 0)
                 );
    
    //printf("shared mem %s '%s' %i \n" ,self->_isWriter? "Writter" : "Reader", GBStringGetCStr(self->_identifier) , self->_associatedKey);
    
    if (( self->_shmid = shmget(self->_associatedKey, self->_size, flags)) < 0)
    {
        printf("Error %s:" , self->_isWriter? "Writter" : "Reader");
        PERROR("shmget");

        return NULL;
    }

    sharedPointer = shmat( self->_shmid, NULL, 0);

    if ( (int)sharedPointer ==  -1)
    {
        PERROR("shmat");
        return NULL;
    }
    
    return sharedPointer;
}

BOOLEAN_RETURN uint8_t Internal_releaseSharedMemory( GBSharedMem* self)
{
    DEBUG_ASSERT( self);
    
    if( self)
    {
        if( shmdt(self->_ptr) == 0 )
        {
            struct shmid_ds buf;
            if( self->_isWriter && shmctl(self->_shmid ,  IPC_RMID , &buf))
            {
                PERROR("shmctl + IPC_RMID:");
                
            }
            return Internal_invalidate(self);
        }
        PERROR("shmdt");
        return 0;
    }
    return 0;
    
}
