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
//  UPCCommons.c
//  GroundBase
//
//  Created by Manuel Deneu on 27/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include <string.h>
#include <GBBinCoder.h>
#include "GBUPC_Private.h"
#include <unistd.h>
#include <sys/socket.h> // getsockname
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>

BOOLEAN_RETURN uint8_t GBUPCServiceProxyGetEndPoint( const GBUPCClientProxy* proxy,GBUPCEndPoint* endPoint)
{
    if( proxy && proxy->_socket && endPoint  )
    {
        const int fd = GBFDSourceGetFileDescriptor(proxy->_socket);
        
        if( fd <= 0)
            return 0;
        
        struct sockaddr addr;
        socklen_t len = sizeof(struct sockaddr);
        
        if(getsockname(fd, &addr,&len) == 0)
        {
            if (addr.sa_family == AF_UNIX)
            {
                
                //DEBUG_ASSERT(len == sizeof(struct sockaddr_un));
                
                const struct sockaddr_un* desc = (const struct sockaddr_un*)&addr;

                endPoint->connectionType = GBUPCTypeLocal;
                endPoint->client.local.name = GBStringInitWithCStr(desc->sun_path);
                
            }
            else if (addr.sa_family == AF_INET)
            {
                DEBUG_ASSERT(len == sizeof(struct sockaddr_in));
                const struct sockaddr_in* desc = (const struct sockaddr_in*)&addr;
                
                endPoint->connectionType = GBUPCTypeDistant;
                
                char ipAddress[INET_ADDRSTRLEN];
                
                if(inet_ntop(AF_INET, &(desc->sin_addr), ipAddress, INET_ADDRSTRLEN))
                {
                    endPoint->client.distant.address = GBStringInitWithCStr(ipAddress);
                }
                else
                {
                    PERROR("GBUPCServiceProxyGetEndPoint.inet_ntop");
                }
                
                
                //endPoint->client.distant.port = ntohl( desc->sin_port );
            }
            else
            {
                DEBUG_ASSERT(0);
            }
            
            return 1;
        }
        else
        {
            PERROR("GBUPCServiceProxyGetEndPoint.getsockname");
            
        }
        
    }
    return 0;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

BOOLEAN_RETURN uint8_t GBUPCMessageSend( GBFDSource* socket , const GBUPCMessage* message )
{
    if( message->dataSize > UPCMessageMaxDataSize)
        return 0;
    
    // we need to copy the header part into a temp structure because message's mtype must be shifted by UPCMessageCode_UserData.
    // and since we're passing the message as a const pointer, we can't directly change it.
    struct _UPCHeader header;
    header.dataSize = message->dataSize;
    header.mtype = UPCMessageCode_UserData + message->mtype;
    
    // send just the header part
    if( GBFDSourceSend(socket, &header, GBUPCMessageHeaderSize, FLAG_NO_SIG_PIPE) == GBUPCMessageHeaderSize)
    {
        if( message->data && message->dataSize)
        {
            return GBFDSourceSend( socket, message->data, message->dataSize, FLAG_NO_SIG_PIPE) == message->dataSize;
        }
        
        return 1;
    }
    
    return 0;
}

BOOLEAN_RETURN uint8_t GBUPCMessageSendPrivate( GBFDSource* socket , const GBUPCMessage* message )
{
    DEBUG_ASSERT( message->dataSize == 0  || message->data);
    if( message->dataSize >UPCMessageMaxDataSize)
        return 0;

    const GBSize sizeToSend = GBUPCMessageHeaderSize + message->dataSize;
    
    return GBFDSourceSend( socket, message, sizeToSend, FLAG_NO_SIG_PIPE) == sizeToSend;
}

BOOLEAN_RETURN uint8_t GBUPCMessageSendObject( GBFDSource* socket , GBRef object ,MsgType messageType )
{
    
    GBBinCoder* coder = GBBinCoderInitWithRootObject(object);
    if( !coder)
        return 0;
    
    GBUPCMessage msg;
    msg.dataSize = (uint32_t) GBBinCoderGetBufferSize(coder);
    msg.mtype = messageType;
    
    msg.data = GBBinCoderGetBuffer(coder);
    
    
    const uint8_t ret = GBUPCMessageSend(socket , &msg);
    GBRelease(coder);
    
    return ret;
}

BOOLEAN_RETURN uint8_t GBUPCMessageRead(  GBFDSource* source , GBUPCMessage *msg )
{
    DEBUG_ASSERT( source);
    DEBUG_ASSERT(msg);
    DEBUG_ASSERT(msg->data);
    
    if( GBSocketReadBlock(source, msg, GBUPCMessageHeaderSize) == 0)
    {
        DEBUG_LOG("[GBUPCMessageRead] GBSocketReadBlock Header error \n");
        return 0;
    }
    
    if( msg->dataSize >= UPCMessageMaxDataSize)
    {
        DEBUG_LOG("[GBUPCMessageRead] Error payload size too big : got %zi where max allowed is %zi \n" , msg->dataSize , UPCMessageMaxDataSize);
        //     ^ that's what he said
        return 0;
    }
    
    if( GBSocketReadBlock(source,/*CONST_CAST*/(void*) msg->data, msg->dataSize))
    {

        return 1;
    }
    else
    {
        DEBUG_LOG("[GBUPCMessageRead] Error reading data \n");
    }
    
    return 0;
    
}

GBBinCoder* GBUPCMessageReadObject( const GBUPCMessage* message)
{
    if( message)
    {
        GBBinCoder* decoder = GBBinCoderInitWithContent(message->data);
        if( decoder)
        {
            return decoder;
        }
    }
    return NULL;
}


