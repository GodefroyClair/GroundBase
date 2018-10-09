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
//  GBUPC_Private.h
//  GroundBase
//
//  Created by Manuel Deneu on 04/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#ifndef GBUPC_Private_h
#define GBUPC_Private_h

#include "../GBObject_Private.h"

#include <GBRunLoop.h>

#include <GBUPCService.h>
#include <GBUPCClient.h>
#include <GBArray.h>

#include <GBSocket.h>
#include "../Private/List.h"


#if defined(__APPLE__) && defined(__MACH__)
#define FLAG_NO_SIG_PIPE SO_NOSIGPIPE
//#define MSG_NOSIGNAL 0
#elif defined(__linux__)
#define FLAG_NO_SIG_PIPE MSG_NOSIGNAL
#endif

#define GBUPCSERVICE_DEFAULT_MAX_CLIENTS (GBSize) 200
/*

#define GBUPC_PREFERED_DEFAULT_SOCKET_TYPE AF_UNIX

#define TIME_TO_WAIT_DURING_PENDING_NS (int) 1000000
#define MAX_PENDING_CONNECTION_ALLOWED (int) 100

#define DEFAULT_TIMEOUT_CLIENT_NS (int) 0
#define DEFAULT_TIMEOUT_CLIENT_S  (int) 5

#define DEFAULT_TIMEOUT_SERVICE_NS (int) 0
#define DEFAULT_TIMEOUT_SERVICE_S  (int) 5

#define SERVICE_FOLDER ( const char*) "/var/tmp/"

#define KEY_NAME_DEFAULT (const char*) "key"

#define KEY_NAME_UPC_DESC (const char*) "Client"
#define KEY_NAME         (const char*) LITTERAL_NAME
#define KEY_PID          (const char*) LITTERAL_PID
*/

#define SERVICE_FOLDER ( const char*) "/var/tmp/"

typedef enum
{
  UPCMessageCode_Error     = 1,
  UPCMessageCode_Accepted  = 1,
    
  UPCMessageCode_UserData  = 100,
    
} UPCMessageCode;

/* **** Service Part ****  */
struct _UPCService
{
    GBObjectBase base;
    GBRunLoop* _runLoop;
    const GBString* name;
    
    GBUPCServiceCallBacks _callbacks;

    int portListen;
    GBFDSource* _tcpListener;
    GBFDSource* _domainListener;
    
    List* _clientProxies;
    //GBArray *clients;
    GBSize  maxClients;

    void* _userContext;
};


struct _UPCClientProxy
{
    GBFDSource* _socket;            // just a ref, not retained
    void*       _userContext;
    GBUPCService *_attachedService;  // just a ref, not retained
    uint8_t _canReadWrite;
    
};


/* **** Client Part ****  */

struct _UPCClient
{
    GBObjectBase base;
    GBRunLoop* _runLoop;
    //const GBString* name;
    GBUPCClientCallBacks _callbacks;
    
    GBFDSource* _socket;
    
    void* _userContext;
};

struct _UPCServiceProxy
{
    GBFDSource* _socket; // just a ref, not retained
};


// just needed in GBUPCMessageSend, but defined here so SystemTests can perform tests to ensure _UPCHeader matches GBUPCMessage structure.
struct _UPCHeader
{
    MsgType    mtype;
    uint32_t dataSize;
};

/* 
 Shared methods between Client & Service. Implemented in UPCCommons.c
 */
BOOLEAN_RETURN uint8_t GBUPCMessageSend( GBFDSource* socket , const GBUPCMessage* message );
BOOLEAN_RETURN uint8_t GBUPCMessageSendObject( GBFDSource* socket , GBRef object ,MsgType messageType );
BOOLEAN_RETURN uint8_t GBUPCMessageRead( GBFDSource* source , GBUPCMessage *msg );

BOOLEAN_RETURN uint8_t GBUPCMessageSendPrivate( GBFDSource* socket , const GBUPCMessage* message );


#endif /* GBUPC_Private_h */
