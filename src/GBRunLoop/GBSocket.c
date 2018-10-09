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
//  GBSocket.c
//  GroundBase
//
//  Created by Manuel Deneu on 26/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include <fcntl.h>

#include <sys/un.h>
#include <unistd.h>
#include <GBSocket.h>
#include "AbstractFileDescriptorSource.h"
#include "UPC/GBUPC_Private.h"


static void Internal_SetReuseFlag( int fd);
static void Internal_SetNoSigPipe( int fd);
//static BOOLEAN_RETURN uint8_t Internal_SetBlocking( int fd ,uint8_t blocking );

GBFDSource* GBTCPSocketCreateListener( int port , GBSize maxClients  , GBRunLoopSourceCallback callback)
{
    if( port < 0 )
        return NULL;
    
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if( fd == -1)
        return NULL;

    struct sockaddr_in si_other;
    
    bzero((char *) &si_other, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_addr.s_addr = INADDR_ANY;
    si_other.sin_port = htons( port );
    
    
    Internal_SetReuseFlag( fd);
    Internal_SetNoSigPipe(fd);
    
    if (bind( fd, (struct sockaddr *) &si_other, sizeof( si_other )) < 0)
    {
        PERROR("[GBTCPSocketCreateListener] bind " );
        
        return NULL;
    }

    if(listen( fd, (int)maxClients) == 0) // OK
    {
        return GBFDSourceInitWithFD(fd, callback);
    }
    
    return NULL;


}




GBFDSource* GBTCPSocketConnectTo( const char*ip , int port ,GBRunLoopSourceCallback callback)
{
    if( ip == NULL || port < 1)
        return NULL;
    
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if( fd == -1)
        return NULL;
    
    struct sockaddr_in serv_addr;
    
    memset(&serv_addr, '0', sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    
    if(inet_pton(AF_INET, ip , &serv_addr.sin_addr)<=0)
    {
        PERROR("inet_pton");
        
        return NULL;
    }
    
    if( connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        PERROR("[GBTCPSocketConnectTo] Error : TCP Connect Failed ");
        
        return NULL;
    }
    
    Internal_SetNoSigPipe(fd);
    
    return GBFDSourceInitWithFD(fd, callback);
}

GBFDSource* GBDomainSocketConnectTo( const char*addr ,GBRunLoopSourceCallback callback)
{
    if( addr == NULL)
        return NULL;
    
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    
    if( fd == -1)
        return NULL;
    
    struct sockaddr_un remote;
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, addr );
    
    const unsigned int len = (unsigned int)( strlen(remote.sun_path)+1 + sizeof(remote.sun_family));
    
    if( connect(fd, (struct sockaddr *)&remote, len ) < 0)
    {
        PERROR("[GBDomainSocketConnectTo] Error : Domain Connect Failed ");
        
        return NULL;
    }
    Internal_SetNoSigPipe(fd);
    
    return GBFDSourceInitWithFD(fd, callback);
    
}

GBFDSource* GBTCPSocketAccept( const GBFDSource* listeningSocket , GBRunLoopSourceCallback callback, struct sockaddr *addr, socklen_t * addrlen)
{
    if( listeningSocket == NULL)
        return NULL;

    int newSock = -1;
    
    //struct sockaddr_in cltAddr;
    //socklen_t c = sizeof( struct sockaddr_in);
    
    newSock = accept( GBFDSourceGetFileDescriptor(listeningSocket) ,addr, addrlen);

    if( newSock > 0)
    {
        Internal_SetNoSigPipe(newSock);
        //Internal_SetBlocking( newSock ,0 );
        return GBFDSourceInitWithFD(newSock, callback);
    }
    
    
    return NULL;
}
GBFDSource* GBDomainSocketAccept( const GBFDSource* listeningSocket , GBRunLoopSourceCallback callback , struct sockaddr *addr, socklen_t * addrlen)
{
    if( listeningSocket == NULL)
        return 0;
    
    int newSock = -1;
    
    newSock = accept(GBFDSourceGetFileDescriptor(listeningSocket), addr, addrlen);
    
    if (newSock)
    {
        Internal_SetNoSigPipe(newSock);
        //Internal_SetBlocking( newSock ,0 );
        return GBFDSourceInitWithFD(newSock, callback);
    }
    return NULL;
}


GBFDSource* GBDomainCreateListener(const char* filePath ,GBSize maxClients  , GBRunLoopSourceCallback callback)
{
    
    if( filePath == NULL)
        return NULL;
    
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    
    struct sockaddr_un local;
    unsigned int len = 0;
    
    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, filePath);
    unlink(local.sun_path);
    len = (unsigned int) strlen(local.sun_path)+1 + sizeof(local.sun_family);
    
    Internal_SetReuseFlag( fd);
    Internal_SetNoSigPipe( fd );
    
    if(bind( fd, (struct sockaddr *)&local, len) < 0)
    {
        PERROR("[GBDomainSocketCreateListener] Bind : ");
        return NULL;
    }
    
    
    
    if(listen( fd, (int)maxClients) == 0) // OK
    {
        return GBFDSourceInitWithFD(fd, callback);
    }
    
    return NULL;
}

static void Internal_SetNoSigPipe( int fd)
{
    int set = 1;
    setsockopt( fd, SOL_SOCKET, FLAG_NO_SIG_PIPE, (void *)&set, sizeof(int));
}
static void Internal_SetReuseFlag( int fd)
{
    
    int set = 1;
    //setsockopt( fd, SOL_SOCKET, FLAG_NO_SIG_PIPE, (void *)&set, sizeof(int));
    setsockopt( fd,SOL_SOCKET,SO_REUSEADDR,&set,sizeof(int));
}
/*
static BOOLEAN_RETURN uint8_t Internal_SetBlocking( int fd ,uint8_t blocking )
{

    int flags = fcntl( fd, F_GETFL, 0);
    
    if (flags < 0)
    {
        return 0;
    }
    
    flags = blocking ? (flags&~O_NONBLOCK) : (flags|O_NONBLOCK);
    
    return fcntl( fd , F_SETFL, flags) == 0;
}
 */

BOOLEAN_RETURN uint8_t GBSocketReadBlock( GBFDSource* source , void* content , GBSize size)
{
    return AbstractFileDescriptorSourceReadBlock(source, content, size);
    
    //return GBRunLoopSourceReadBlock(source, content, size);
}
BOOLEAN_RETURN uint8_t GBSocketWriteBlock( GBFDSource *source , const void* data , GBSize dataLength)
{
    return AbstractFileDescriptorSourceSendBlock(source,data, dataLength , FLAG_NO_SIG_PIPE);
    //return GBRunLoopSourceSendBlock(source, data, dataLength , FLAG_NO_SIG_PIPE);
}



