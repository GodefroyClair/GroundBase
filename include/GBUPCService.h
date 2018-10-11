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
//  GBUPCService.h
//  GroundBase
//
//  Created by Manuel Deneu on 02/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//



/**
 * \file GBUPCService.h
 * \brief GBUPCService C API. See GBUPCService.hpp for C++ API.
 * GBUPCServices : Unified Process Services, enabling communication between local or distant processes.
 * In short, services can be accessed by name (local) or hostname+port (distant).
 * Rules about whether to connect fisrt to local or distant are set in GBUPCServicesPreferences
 */


#ifndef GBUPCService_h
#define GBUPCService_h




#include <GBTypes.h>
#include <GBObject.h>
#include <GBString.h>
#include <GBRunLoop.h>
#include <GBUPC.h>
#include <sys/types.h> // uid_t gid_t

GB_BEGIN_DCL

extern GBObjectClassRef GBUPCServiceClass;
#define GBUPCServiceClassName (const char*) "GBUPCService"

extern GBSize GBUPCService_Size;
/*!
 * @discussion An opaque GBUPCService instance.
 */
typedef struct _UPCService GBUPCService;

typedef enum
{
    GBUPCDisconnectedByService = 1, // connection was closed by Service
    GBUPCDisconnectedByClient  = 2, // connection was closed by client
}GBUPCDisconnectionReason;


/*
 called when a client passes data to the service.
 \param service The concerned service
 \param client The client sending data
 \param data   The data
 */
typedef void (*GBUPCServiceDidReceiveData)( GBUPCService* service, const GBUPCClientProxy* client, const GBUPCMessage* data  );


/*
 called when a client wants connection
 \param service The concerned service
 \param client The  client requesting connection. Note, you can only get credentials infos about the client during this callback. All read/write operations are prohibed. You can also call GBUPCClientProxy[Get/Set]UserContext
 \return 1 to accept, 0 to refuse
 */
typedef BOOLEAN_RETURN uint8_t (*GBUPCServiceClientConnectionRequest) ( GBUPCService* service ,GBUPCClientProxy* client );


/*
 called when a client is disconnected
 \param service The concerned service
 \param client the disconnected client.Note, you can only get credentials infos about the client during this callback. All read/write operations are prohibed. This is also your last chance to get the context pointer from `GBUPCClientProxyGetUserContext`
 */
typedef void (*GBUPCServiceClientDisconnected) ( GBUPCService* service , const GBUPCClientProxy* client , GBUPCDisconnectionReason reason );


/*!
 * @discussion GBUPCServiceCallBacks serves as a parameter for `GBUPCServiceSetCallBacks` to set GBUPC Service callbacks.
 */
typedef struct
{
    GBUPCServiceDidReceiveData          dataCallBack;              // required
    GBUPCServiceClientConnectionRequest connectionRequestCallBack; // required
    GBUPCServiceClientDisconnected      disconnectionCallBack;     // required
    
} GBUPCServiceCallBacks;



/*
 Will fail if serviceName is NULL or empty
 On sucess ,will retain serviceName
 */
GBUPCService* GBUPCServiceInitWithName(const GBString* serviceName );
    


void GBUPCServiceSetCallBacks( GBUPCService* service ,GBUPCServiceCallBacks callbacks );

// will fail if service is running
// Port must be >=0. if zero, a random port will be assigned
BOOLEAN_RETURN uint8_t GBUPCServiceSetListeningPort( GBUPCService* service , int port );

/* Getters*/
const GBString* GBUPCServiceGetName( const GBUPCService* service);
GBSize GBUPCServiceGetNumClients( const GBUPCService* service);

GBRunLoop* GBUPCServiceGetRunLoop( GBUPCService* service);
BOOLEAN_RETURN uint8_t GBUPCServiceSetRunLoop( GBUPCService* service , GBRunLoop* runLoop);

// callbacks must be set!
// if you plan on using an external runloop, it should be set using `GBUPCServiceSetRunLoop` prior of calling Start.
BOOLEAN_RETURN uint8_t GBUPCServiceStart( GBUPCService* service);

BOOLEAN_RETURN uint8_t GBUPCServiceIsRunning( const GBUPCService* service);

void GBUPCServiceSetUserContext(GBUPCService* service,  void* ptr);
void* GBUPCServiceGetUserContext(const GBUPCService* service);

/* GBUPCClientProxy */

BOOLEAN_RETURN uint8_t GBUPCServiceCloseAndRemoveClient( GBUPCService* service , const GBUPCClientProxy* client);

BOOLEAN_RETURN uint8_t GBUPCServiceSendMessage( GBUPCService* service , const GBUPCClientProxy* client , const GBUPCMessage* msg);
BOOLEAN_RETURN uint8_t GBUPCServiceSendObject( GBUPCService* service , const GBUPCClientProxy* client ,GBRef object , MsgType messageType );

BOOLEAN_RETURN uint8_t GBUPCServiceBroadcastMessage( GBUPCService* service ,  const GBUPCMessage* msg);
BOOLEAN_RETURN uint8_t GBUPCServiceBroadcastObject( GBUPCService* service , GBRef object , MsgType messageType );

void GBUPCClientProxySetUserContext( GBUPCClientProxy* proxy , void* ptr);
void* GBUPCClientProxyGetUserContext( const GBUPCClientProxy* proxy);

GBUPCService* GBUPCClientProxyGetService( GBUPCClientProxy* proxy);
    
    
    // Note: this is only valid for local clients.
typedef struct
{
    uid_t clientUID;
    gid_t clientGID;
    pid_t clientPID;
} GBUPCClientCredentials;
    
// Not filling pid for now.
BOOLEAN_RETURN uint8_t GBUPCServiceGetClientCredentials( GBUPCService* service , const GBUPCClientProxy* client , GBUPCClientCredentials* credentials );
    
GB_END_DCL

#endif /* GBUPCService_h */
