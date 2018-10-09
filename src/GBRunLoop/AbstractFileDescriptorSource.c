/*
 * Copyright (c) 2017 FlyLab
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
//  AbstractFileDescriptorSource.c
//  GroundBase
//
//  Created by Manuel Deneu on 25/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>

#include "AbstractFileDescriptorSource.h"

BOOLEAN_RETURN uint8_t AbstractFileDescriptorSourceInit(AbstractFileDescriptorSource* self , GBRunLoopSourceCallback callback)
{
    if( self == NULL)
        return 0;
    
    self->_fd = UNINITIALIZED_FD;
    
    self->closeOnDestruct = 0;
    
    if( AbstractRunLoopSourceInit((AbstractRunLoopSource*)self  , callback))
    {
        return 1;
    }
    
    
    return 0;
}

int AbstractFileDescriptorSourceGetFD( const AbstractFileDescriptorSource* self)
{
    return self->_fd;
}

GBSize AbstractFileDescriptorSourceRead( AbstractFileDescriptorSource* self , void* content , GBSize size)
{
    DEBUG_ASSERT(self);
    
    
    const ssize_t ret  = read(self->_fd, content, size);
    
    if( ret >=0)
        return (GBSize) ret;
    
    PERROR("AbstractFileDescriptorSourceRead.read");
    return GBSizeInvalid;
}

GBSize AbstractFileDescriptorSourceSend( AbstractFileDescriptorSource *source , const void* data , GBSize dataLength , int flags)
{
    DEBUG_ASSERT(source);
    DEBUG_ASSERT(data);
    
    
    const ssize_t ret = send( source->_fd , data , dataLength , flags);
    
    if( ret == -1)
    {
        PERROR("AbstractFileDescriptorSourceSend.send");
        return 0;
    }
    
    return (GBSize) ret;
}

GBSize AbstractFileDescriptorSourceWrite( AbstractFileDescriptorSource *source , const void* data , GBSize dataLength )
{
    DEBUG_ASSERT(source);
    DEBUG_ASSERT(data);
    
    const ssize_t ret = write( source->_fd , data , dataLength );
    
    if( ret == -1)
    {
        PERROR("AbstractFileDescriptorSourceSend.write");
        return GBSizeInvalid;
    }
    
    return (GBSize) ret;
    
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

BOOLEAN_RETURN uint8_t AbstractFileDescriptorSourceReadBlock( AbstractFileDescriptorSource* source , void* content , GBSize size)
{
    if( source == NULL)
    {
        return 0;
    }

    size_t    bytesRead = 0;
    
    while(bytesRead != size)
    {
        ssize_t didRead = 0;
        
        do
        {
            didRead = read( source->_fd ,(uint8_t*) content + bytesRead, (size - bytesRead) );
            
            if( didRead == -1)
            {
                PERROR("didread");
            }
            if( didRead == 0)
            {
                //source->source._callback(source , GBRunLoopSourceError);
                //DEBUG_LOG("Nothing to read \n");
                return 0;
            }
        } while( (didRead == -1 ) && (errno == EINTR));
        
        if( didRead  == -1)
        {
            PERROR("read");
            source->source._callback(source , GBRunLoopSourceError);
            //GBrunLoopNotify( source , GBRunLoopSourceErrorRead);
            return 0;
        }
        bytesRead += ( size_t) didRead;
    }
    
    return 1;
}


BOOLEAN_RETURN uint8_t AbstractFileDescriptorSourceWriteBlock( AbstractFileDescriptorSource *source , const void* data , GBSize dataLength)
{
    if(source == NULL)
        return 0;
    
    ssize_t written = 0;
    
    while (written !=  (ssize_t) dataLength)
    {
        ssize_t writtenThisTime = 0;
        do
        {
            writtenThisTime = write(source->_fd ,(char*) data + written , (dataLength - (GBSize) written ) );
            //int err = errno;
            if( writtenThisTime == 0)
            {
                source->source._callback(source , GBRunLoopSourceError);
                //GBrunLoopNotify(source, GBRunLoopSourceErrorWrite);
                return 0;
            }
        } while (written == -1 && errno == EINTR);
        
        
        if( written == -1)
        {
            source->source._callback(source , GBRunLoopSourceError);
            //GBrunLoopNotify(source, GBRunLoopSourceErrorWrite);
            return 0;
        }
        
        written += writtenThisTime;
    }
    
    return written == (ssize_t)dataLength;
}

BOOLEAN_RETURN uint8_t AbstractFileDescriptorSourceSendBlock( AbstractFileDescriptorSource *source , const void* data , GBSize dataLength , int flags)
{
    if(source == NULL)
        return 0;

    ssize_t written = 0;
    
    
    while (written !=  (ssize_t) dataLength)
    {
        ssize_t writtenThisTime = 0;
        do
        {
            writtenThisTime = send( source->_fd ,(char*) data + written , (dataLength - (GBSize) written ) , flags );
            //int err = errno;
            if( writtenThisTime == 0)
            {
                source->source._callback(source , GBRunLoopSourceError);
                //GBrunLoopNotify(source, GBRunLoopSourceErrorWrite);
                return 0;
            }
        } while (written == -1 && errno == EINTR);
        
        
        if( written == -1)
        {
            source->source._callback(source , GBRunLoopSourceError);
            //GBrunLoopNotify(source, GBRunLoopSourceErrorWrite);
            return 0;
        }
        
        written += writtenThisTime;
    }
    
    return written == (ssize_t)dataLength;
}
