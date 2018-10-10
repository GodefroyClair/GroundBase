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
//  GBSocket.h
//  GroundBase
//
//  Created by Manuel Deneu on 26/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

/**
 * \file GBSocket.h
 * \brief GBSocket is an alias for a GBFDSource, specialized for socket operations.
 */

#ifndef GBSocket_h
#define GBSocket_h



#include <sys/socket.h> // socklen_t TODO : remove from public headers
#include <GBFDSource.h>

GB_BEGIN_DCL

/*
 Creates and returns a socket that listens for TCP connections on a socket.
 maxClients :  maximum length for the queue of pending connections. See man listen(2) for underlying implementation.
 port       : a >0 number. If = 0 a port will be assigned by the kernel.
 callback   : can be NULL
 */
GBFDSource* GBTCPSocketCreateListener( int port , GBSize maxClients  , GBRunLoopSourceCallback callback);
    
/*
 Initiates a connection on a TCP Socket
 
 */
GBFDSource* GBTCPSocketConnectTo( const char*ip , int port ,GBRunLoopSourceCallback callback);

GBFDSource* GBTCPSocketAccept( const GBFDSource* listeningSocket , GBRunLoopSourceCallback callback , struct sockaddr *addr, socklen_t * addrlen);
GBFDSource* GBDomainSocketAccept( const GBFDSource* listeningSocket , GBRunLoopSourceCallback callback , struct sockaddr *addr, socklen_t * addrlen);

/*
 Creates and returns a socket that listens for Unix connections on a socket.
 maxClients :  maximum length for the queue of pending connections. See man listen(2) for underlying implementation.
 filePath   : a valid file path. Will return NULL if NULL
 callback   : can be NULL
 */
GBFDSource* GBDomainCreateListener(const char* filePath , GBSize maxClients  ,GBRunLoopSourceCallback callback);
    
/*
 filePath   : a valid file path. Will return NULL if NULL
 callback   : can be NULL
 */
GBFDSource* GBDomainSocketConnectTo( const char*addr ,GBRunLoopSourceCallback callback);
    
BOOLEAN_RETURN uint8_t GBSocketWriteBlock( GBFDSource *source , const void* data , GBSize dataLength);
BOOLEAN_RETURN uint8_t GBSocketReadBlock( GBFDSource* source , void* content , GBSize size);
    
GB_END_DCL

#endif /* GBSocket_h */
