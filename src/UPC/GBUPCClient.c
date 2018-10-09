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
//
//  GBUPCClient.c
//  GroundBase
//
//  Created by Manuel Deneu on 03/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#include <string.h> // memset
#include <GBUPCClient.h>

#include <GBUPC.h>
#include "GBUPC_Private.h"

#include "../GBObject_Private.h"


static BOOLEAN_RETURN uint8_t Internal_PrepareClient( GBUPCClient* client);
static BOOLEAN_RETURN uint8_t Internal_ConnectToTCP( GBUPCClient* client , const char* ip , int port);
static BOOLEAN_RETURN uint8_t Internal_ConnectToDomain( GBUPCClient* client , const char* addr);
static  void onClientSocket( GBRunLoopSource* source , GBRunLoopSourceNotification notification);

static void * GBUPCClientCtor(void * _self, va_list * app);
static void * GBUPCClientDtor (void * _self);
static uint8_t  GBUPCClientEquals (const void * _self, const void * _b);
static GBRef GBUPCClientGetDescription (const void * self);

static const GBObjectClass _GBUPCClientClass =
{
    sizeof(struct _UPCClient),
    GBUPCClientCtor,
    GBUPCClientDtor,
    NULL,
    GBUPCClientEquals,
    GBUPCClientGetDescription,
    NULL, // init
    NULL, // deinit
    NULL, //retain
    NULL, // release
    (char*)GBUPCClientClassName
};

GBObjectClassRef GBUPCClientClass = & _GBUPCClientClass;


static void * GBUPCClientCtor(void * _self, va_list * app)
{
    UNUSED_PARAMETER(app);
    GBUPCClient* self = _self;


    self->_runLoop = NULL;
    self->_socket = NULL;
    
    self->_userContext = NULL;
    
    return self;

}
static void * GBUPCClientDtor (void * _self)
{
    
    GBUPCClient* self = _self;

    if( self->_runLoop)
    {
        GBRelease(self->_runLoop);
    }
    if( self->_socket)
    {
        GBRelease(self->_socket);
    }
   
    return self;
}

static uint8_t  GBUPCClientEquals (const void * _self, const void * _b)
{
    UNUSED_PARAMETER(_self);
    UNUSED_PARAMETER( _b);
    return 0;
}
static GBRef GBUPCClientGetDescription (const void * _self)
{
    const GBUPCClient* self = _self;
    if( self)
    {
        return NULL;
    }
    
    return  NULL;
}

/*
GBUPCClient* GBUPCClientInit()
{
    return GBObjectAlloc( GBDefaultAllocator,GBUPCClientClass);
}
*/
GBUPCClient* GBUPCClientInit( GBUPCClientCallBacks callbacks )
{
    
    
    GBUPCClient* client = GBObjectAlloc( GBDefaultAllocator,GBUPCClientClass);
    
    if( client )
    {

        
        client->_callbacks.dataCallBack = callbacks.dataCallBack;
        client->_callbacks.notificationCallback = callbacks.notificationCallback;
        
        client->_runLoop = NULL;

    }
    
    return client;
}


void GBUPCClientSetUserContext(GBUPCClient* client,  void* ptr)
{
    if( client)
    {
        client->_userContext = ptr;
    }
}

void* GBUPCClientGetUserContext(const GBUPCClient* client)
{
    if( client)
    {
        return client->_userContext;
    }
    
    return NULL;
}



GBRunLoop* GBUPCClientGetRunLoop( GBUPCClient* client)
{
    return client->_runLoop;
}
BOOLEAN_RETURN uint8_t GBUPCClientSetRunLoop( GBUPCClient* client , GBRunLoop* runLoop)
{
    if( client && runLoop)
    {
        if( client->_runLoop)
        {
            GBRelease(client->_runLoop);
            client->_runLoop = NULL;
        }
        
        DEBUG_ASSERT(client->_runLoop == NULL);
        
        client->_runLoop = runLoop;
        GBRetain(runLoop);
        
    }
    
    return 0;
}

BOOLEAN_RETURN uint8_t GBUPCClientDisconnect( GBUPCClient* client)
{
    if( client && GBUPCClientIsConnected(client))
    {
        DEBUG_ASSERT(client->_socket != NULL);
        
        
        GBRunLoopRemoveSource(client->_runLoop, client->_socket);
        GBRelease(client->_socket);
        client->_socket = NULL;
        
        //  client->_callbacks.notificationCallback(client , GBUPCNotificationDisconnected);
        
        return 1;
        
    }
    return 0;
}

BOOLEAN_RETURN uint8_t GBUPCClientConnectToEndPoint( GBUPCClient* client , const GBUPCEndPoint *endPoint)
{
    if (client && endPoint && GBUPCEndPointIsValid(endPoint) )
    {
        if( client && Internal_PrepareClient(client))
        {
            DEBUG_ASSERT(client->_runLoop);
            
            uint8_t ret = 0;
            if( endPoint->connectionType == GBUPCTypeLocal)
            {
                ret = Internal_ConnectToDomain(client, GBStringGetCStr( endPoint->client.local.name) );
            }
            else if( endPoint->connectionType == GBUPCTypeDistant)
            {
                ret = Internal_ConnectToTCP( client , GBStringGetCStr( endPoint->client.distant.address ) , endPoint->client.distant.port);
            }
            if( ret)
            {
                GBRunLoopAddSource(client->_runLoop, client->_socket);
                
                return 1;//GBRunLoopRun(client->_runLoop);
            }
            
        }

    }
    return 0;
}


BOOLEAN_RETURN uint8_t GBUPCClientConnectToLocalEndPoint( GBUPCClient* client , const char *name)
{
    GBUPCEndPoint endpoint;
    GBUPCEndPointInit(&endpoint);
    endpoint.connectionType = GBUPCTypeLocal;
    endpoint.client.local.name = GBStringInitWithCStr(name);
    
    const uint8_t ret = GBUPCClientConnectToEndPoint(client, &endpoint);
    
    GBUPCEndPointRelease(&endpoint);
    return ret;
}
BOOLEAN_RETURN uint8_t GBUPCClientConnectToTCPEndPoint( GBUPCClient* client , const char *ip , int port)
{
    GBUPCEndPoint endpoint;
    GBUPCEndPointInit(&endpoint);
    endpoint.connectionType = GBUPCTypeDistant;
    endpoint.client.distant.address = GBStringInitWithCStr(ip);
    endpoint.client.distant.port = port;
    
    const uint8_t ret = GBUPCClientConnectToEndPoint(client, &endpoint);
    
    GBUPCEndPointRelease(&endpoint);
    return ret;
    
}

// callbacks must be set!
/*
BOOLEAN_RETURN uint8_t GBUPCClientConnectToTCP( GBUPCClient* client , const char* ip , int port)
{
    if( ip == NULL || port <=0)
    {
        return 0;
    }
    
    if( client && Internal_PrepareClient(client))
    {
        DEBUG_ASSERT(client->_runLoop);
        
        if( Internal_ConnectToTCP( client , ip , port))
        {
            GBRunLoopAddSource(client->_runLoop, client->_socket);

            return GBRunLoopRun(client->_runLoop);
        }
    }
    return 0;
}

BOOLEAN_RETURN uint8_t GBUPCClientConnectToDomain( GBUPCClient* client , const char* addr)
{
    if( addr == NULL)
        return 0;
    
    if( client && Internal_PrepareClient(client))
    {
        DEBUG_ASSERT(client->_runLoop);
        if (Internal_ConnectToDomain(client, addr))
        {
            GBRunLoopAddSource(client->_runLoop, client->_socket);
            
            return GBRunLoopRun(client->_runLoop);
            
        }
        
    }
    
    return 0;
}
 */

BOOLEAN_RETURN uint8_t GBUPCClientRun( GBUPCClient* client)
{
    return GBRunLoopRun(client->_runLoop);
}

BOOLEAN_RETURN uint8_t GBUPCClientIsRunning( const GBUPCClient* client)
{
    if( client->_runLoop)
    {
        return client->_runLoop != NULL;
    }
    return 0;
}

BOOLEAN_RETURN uint8_t GBUPCClientIsConnected( const GBUPCClient* client)
{
    if( client && client->_socket)
    {
        return GBFDSourceGetFileDescriptor(client->_socket) != UNINITIALIZED_FD;
    }
    return 0;
}

static BOOLEAN_RETURN uint8_t Internal_PrepareClient( GBUPCClient* client)
{
    DEBUG_ASSERT(client);

    GBUPCClientDisconnect(client);
    
    
    DEBUG_ASSERT(client->_socket == NULL);
    
    if( client->_runLoop == NULL)
    {
        client->_runLoop = GBRunLoopInit();
    }
    return client->_runLoop != NULL;
}

static BOOLEAN_RETURN uint8_t Internal_ConnectToDomain( GBUPCClient* client , const char* addr)
{
    DEBUG_ASSERT(client);
    DEBUG_ASSERT(addr);
    DEBUG_ASSERT(client->_socket == NULL);
    
    const GBString* fullAddr = GBStringInitWithFormat("%s%s" , SERVICE_FOLDER , addr);
    
    client->_socket = GBDomainSocketConnectTo( GBStringGetCStr(fullAddr) , onClientSocket);
    
    if( client->_socket)
    {
        GBFDSourceShouldCloseOnDestruct(client->_socket ,1);
        GBRunLoopSourceSetUserContext(client->_socket, client);
    }
    else
    {
        DEBUG_ERR("GBUPClient Error Domain : unable to connect to '%s' \n"  ,addr );
    }
    GBRelease(fullAddr);
    
    return client->_socket != NULL;
}

static BOOLEAN_RETURN uint8_t Internal_ConnectToTCP( GBUPCClient* client , const char* ip , int port)
{
    DEBUG_ASSERT(client);
    DEBUG_ASSERT(ip);
    DEBUG_ASSERT(client->_socket == NULL);
    
    client->_socket = GBTCPSocketConnectTo(ip , port ,onClientSocket );
    
    if( client->_socket)
    {
        GBFDSourceShouldCloseOnDestruct(client->_socket ,1);
        GBRunLoopSourceSetUserContext(client->_socket, client);
    }
    else
    {
        DEBUG_ERR("GBUPClient Error TCP : unable to connect to '%s' on port %i\n"  ,ip , port );
    }
    return client->_socket != NULL;
}



BOOLEAN_RETURN uint8_t GBUPCClientSendMessage( GBUPCClient* client , const GBUPCMessage* message )
{
    if( client && client->_socket && message)
    {

        return GBUPCMessageSend( client->_socket , message );
        
    }
    return 0;
}

BOOLEAN_RETURN uint8_t GBUPCClientSendObject( GBUPCClient* client , GBRef object , MsgType messageType )
{
    if( client && client->_socket && object)
    {
        return GBUPCMessageSendObject(client->_socket,object ,messageType);
        
    }
 
    return 0;
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* Callbacks */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

static void Internal_RemoveSource( GBRunLoopSource* source)
{
    GBUPCClient* client = GBRunLoopSourceGetUserContext(source);
    DEBUG_ASSERT(client);
    client->_callbacks.notificationCallback(client , GBUPCNotificationDisconnected);
    
    if(GBRunLoopRemoveSource(client->_runLoop, client->_socket) == 0)
    {
        //DEBUG_ASSERT(0);
    }
    
    if(GBObjectGetRefCount(client->_socket) == 1)
    {
        GBRelease(client->_socket);
    
    }
    else
    {
        DEBUG_LOG("client->_socket invalid ref count!");
    }

    
    client->_socket = NULL;
}

/*
 TODO : This method is NOT REENTRANT (static variables)
 */
static  void onClientSocket( GBRunLoopSource* source , GBRunLoopSourceNotification notification)
{
    DEBUG_ASSERT(source);
    GBUPCClient* client = GBRunLoopSourceGetUserContext(source);
    DEBUG_ASSERT(client);
    DEBUG_ASSERT(source == client->_socket);

    switch (notification)
    {
        case GBRunLoopSourceError:
        case GBRunLoopSourceDisconnected:
        {
            Internal_RemoveSource(source);
            
        }
            break;
            
        case GBRunLoopSourceCanRead:
        {
            static GBUPCMessage msg;
            static uint8_t inBuffer[ UPCMessageMaxDataSize];
            
            memset(&msg, 0, sizeof(GBUPCMessage));
            memset(inBuffer, 0, sizeof(UPCMessageMaxDataSize));
            
            msg.data = inBuffer; // pointer ref
            
            if(GBUPCMessageRead( source ,&msg ))
            {
                if( msg.mtype < UPCMessageCode_UserData)
                {
                    if(msg.mtype == UPCMessageCode_Accepted)
                    {
                        client->_callbacks.notificationCallback(client , GBUPCNotificationConnected);
                    }
                    
                }
                else
                {
                    msg.mtype -= UPCMessageCode_UserData;
                    client->_callbacks.dataCallBack(client , &msg);
                }
            }
            else
            {
                Internal_RemoveSource(source);
            }
        }
            break;
        default:
            //("Client socket notification %i\n" , notification);
            DEBUG_ASSERT(0 && "[GBUPC Client] Unhandled Socket notification"); // to inspect
            break;
    }
    
    
}
