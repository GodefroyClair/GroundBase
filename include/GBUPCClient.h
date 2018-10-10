/*
 * Copyright (c) 2016-2018 FlyLab
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
//  GBUPCClient.h
//  GroundBase
//
//  Created by Manuel Deneu on 03/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

/**
 * \file GBUPCClient.h
 * \brief GBUPCClient C API. See GBUPCClient.hpp for C++ API
 */


#ifndef GBUPCClient_h
#define GBUPCClient_h


#include <GBObject.h>
#include <GBString.h>
#include <GBUPC.h>
#include <GBRunLoop.h>
#include <GBUPCEndPoint.h>

GB_BEGIN_DCL

extern GBObjectClassRef GBUPCClientClass;
#define GBUPCClientClassName (const char*) "GBUPCClient"

/*!
 * @discussion An opaque GBUPCClient instance.
 */
typedef struct _UPCClient GBUPCClient;

typedef enum
{
    GBUPCNotificationPending            = 1,
    GBUPCNotificationConnected          = 2,
    //GBUPCNotificationConnectionRefused  = 3,
    GBUPCNotificationDisconnected       = 4,
    //GBUPCNotificationTimeout            = 5,
    //GBUPCNotificationWarning            = 6,
    
} GBUPCNotification;



/*
 called when a client passes data to the service.
 \param service The concerned service
 \param client The client sending data
 \param data   The data
 */

typedef void (*GBUPCClientDidReceiveData)( GBUPCClient* client,  const GBUPCMessage* data  );
typedef void (*GBUPCClientNotification) ( GBUPCClient* client , GBUPCNotification notification );

    
/*!
 * @discussion GBUPCClientCallBacks serves as a parameter for `GBUPCClientInit` to set GBUPC Client callbacks.
 */
typedef struct
{
    GBUPCClientDidReceiveData    dataCallBack;         // required
    GBUPCClientNotification      notificationCallback; // required
    
} GBUPCClientCallBacks;


/*!
 * @discussion Creates a new GBUPCClient instance.
 * @param callbacks a GBUPCClientCallBacks instance containing the data & notification callbacks.
 * @return A fully initialized instance of GBUPCClient, of type `GBUPCUnknown`.
 */
GBUPCClient* GBUPCClientInit(  GBUPCClientCallBacks callbacks );

    // client must be valid!
GBRunLoop* GBUPCClientGetRunLoop( GBUPCClient* client);
    
// client and runloop must be valid, ie. you can't pass NULL runLoop.
BOOLEAN_RETURN uint8_t GBUPCClientSetRunLoop( GBUPCClient* client , GBRunLoop* runLoop);


// connect to the service, but will no run the runLoop
// on success, call `GBRunLoopRun` on the client' runloop.
BOOLEAN_RETURN uint8_t GBUPCClientConnectToEndPoint( GBUPCClient* client , const GBUPCEndPoint *endPoint);
    
/*
 Convenience method to connect to a local endoint. is equivalent to call `GBUPCClientConnectToEndPoint` with a GBUPCTypeLocal type GBUPCEndPoint.
 */
BOOLEAN_RETURN uint8_t GBUPCClientConnectToLocalEndPoint( GBUPCClient* client , const char *name);
    
/*
 Convenience method to connect to a tcp endoint. is equivalent to call `GBUPCClientConnectToEndPoint` with a GBUPCTypeDistant type GBUPCEndPoint.
 */
BOOLEAN_RETURN uint8_t GBUPCClientConnectToTCPEndPoint( GBUPCClient* client , const char *ip , int port);

// close service connection. Will not stop the runloop
BOOLEAN_RETURN uint8_t GBUPCClientDisconnect( GBUPCClient* client);
// callbacks must be set!
// will return 0 if unable to connect, or directly run the runloop. GBUPCClientNotification callback will be called with GBUPCNotificationConnected notification on sucess.
//BOOLEAN_RETURN uint8_t GBUPCClientConnectToTCP( GBUPCClient* client , const char* ip , int port);
//BOOLEAN_RETURN uint8_t GBUPCClientConnectToDomain( GBUPCClient* client , const char* addr);

BOOLEAN_RETURN uint8_t GBUPCClientRun( GBUPCClient* client);

GB_DEPRECATED("5.3.6") BOOLEAN_RETURN uint8_t GBUPCClientIsRunning( const GBUPCClient* client);
    
BOOLEAN_RETURN uint8_t GBUPCClientIsConnected( const GBUPCClient* client);


void GBUPCClientSetUserContext(GBUPCClient* client,  void* ptr);
void* GBUPCClientGetUserContext(const GBUPCClient* client);

BOOLEAN_RETURN uint8_t GBUPCClientSendMessage( GBUPCClient* client , const GBUPCMessage* message );
BOOLEAN_RETURN uint8_t GBUPCClientSendObject( GBUPCClient* client , GBRef object , MsgType messageType );

GB_END_DCL

#endif /* GBUPCClient_h */
