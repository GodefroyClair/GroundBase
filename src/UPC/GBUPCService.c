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
//  GBUPCService.c
//  GroundBase
//
//  Created by Manuel Deneu on 02/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//


#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>

#include <GBString.h>


#include "../GBObject_Private.h"
#include "../GBRunLoop/AbstractFileDescriptorSource.h"
#include "GBUPC_Private.h"

static BOOLEAN_RETURN uint8_t Internal_PrepareService( GBUPCService* service);
static BOOLEAN_RETURN uint8_t Internal_CreateListenners( GBUPCService* service);
static BOOLEAN_RETURN uint8_t Internal_GBUPCServiceRespondToConnectionRequest( GBUPCService* service , GBRunLoopSource* socket);

#ifdef DEBUG
static void Internal_Check( const GBUPCService* service);
#endif
static GBUPCClientProxy* GetProxyBySource( const GBUPCService* service , const GBRunLoopSource* source);

// callbacks
static  void onListenner( GBRunLoopSource* source , GBRunLoopSourceNotification notification);
static  void onClient( GBRunLoopSource* source , GBRunLoopSourceNotification notification);

static void * GBUPCServiceCtor(void * _self, va_list * app);
static void * GBUPCServiceDtor (void * _self);
static uint8_t  GBUPCServiceEquals (const void * _self, const void * _b);
static GBRef GBUPCServiceGetDescription (const void * self);

static const GBObjectClass _GBUPCServiceClass =
{
    sizeof(struct _UPCService),
    GBUPCServiceCtor,
    GBUPCServiceDtor,
    NULL,
    GBUPCServiceEquals,
    GBUPCServiceGetDescription,
    NULL, // init
    NULL, // deinit
    NULL,
    
    NULL,
    (char*)GBUPCServiceClassName
};

GBSize GBUPCService_Size = sizeof( struct _UPCService );


GBObjectClassRef GBUPCServiceClass = & _GBUPCServiceClass;

static void * GBUPCServiceCtor(void * _self, va_list * app)
{
    GBUPCService* self = _self;

    const GBString* serviceName = va_arg(* app, const GBString*);
    
    if( serviceName == NULL)
        return NULL;

    self->_tcpListener    = NULL;
    self->_domainListener = NULL;
    
    GBRetain(serviceName);
    self->name = serviceName;// GBObjectClone(serviceName);
    
    memset(&self->_callbacks, 0, sizeof(GBUPCServiceCallBacks));


    self->_runLoop = NULL;

    self->_clientProxies  = ListInit();
    
    self->maxClients = GBUPCSERVICE_DEFAULT_MAX_CLIENTS;
    
    self->portListen = UNINITIALIZED_FD;
    
    
    memset(&self->_callbacks, 0, sizeof(GBUPCServiceCallBacks));
    

    return self;
}
static void * GBUPCServiceDtor (void * _self)
{
    GBUPCService* self = _self;
        
    if( self->_runLoop)
    {
        GBRelease(self->_runLoop);
    }
    GBRelease(self->name);
    
    void* clts = NULL;
    ListForEach(self->_clientProxies, clts)
    {
        GBUPCClientProxy* prox = clts;
        DEBUG_ASSERT(prox);
        GBRelease(prox->_socket);
        
    }
    ListRemoveAll(self->_clientProxies);
    ListFree(self->_clientProxies);
    

    if( self->_domainListener)
    {
        GBRelease(self->_domainListener);
    }
    if( self->_tcpListener)
    {
        GBRelease(self->_tcpListener);
    }
    
    
    
    
    return self;
}


static uint8_t  GBUPCServiceEquals (const void * _self, const void * _b)
{
    UNUSED_PARAMETER(_self);
    UNUSED_PARAMETER(_b);
    return 0;
}
static GBRef GBUPCServiceGetDescription (const void * _self)
{
    const GBUPCService* self = _self;
    if( self)
    {
        return GBStringInitWithFormat("ID '%s' %zi client(s) " ,
                                      "name",//GBStringGetCStr(self->name ) ,
                                      0//GBUPCServiceGetClientsCount(self)
                                      );
    }
    
    return NULL;
}


GBUPCService* GBUPCServiceInitWithName_OnStack(const GBString* serviceName , uint8_t* buffer )
{
    
    return GBObjectAlloc(GBStackAllocator, GBUPCServiceClass , serviceName);
}

GBUPCService* GBUPCServiceInitWithName(const GBString* serviceName )
{
    if( serviceName == NULL || GBStringIsEmpty(serviceName))
        return NULL;
    
    return GBObjectAlloc( GBDefaultAllocator,GBUPCServiceClass , serviceName);
}

void GBUPCServiceSetCallBacks( GBUPCService* service ,GBUPCServiceCallBacks callbacks )
{
    if( service == NULL)
        return;
    
    service->_callbacks = callbacks;
}

BOOLEAN_RETURN uint8_t GBUPCServiceSetListeningPort( GBUPCService* service , int port )
{
    if( service && !GBUPCServiceIsRunning(service) && port >= 0)
    {
        service->portListen = port;
        
        return 1;
    }
    
    return 0;
}

const GBString* GBUPCServiceGetName( const GBUPCService* service)
{
    return service->name;
}

GBSize GBUPCServiceGetNumClients( const GBUPCService* service)
{
    return ListGetSize(service->_clientProxies);
}

GBRunLoop* GBUPCServiceGetRunLoop( GBUPCService* service)
{
    return service->_runLoop;
}

BOOLEAN_RETURN uint8_t GBUPCServiceSetRunLoop( GBUPCService* service , GBRunLoop* runLoop)
{
    if( service && runLoop)
    {
        if( service->_runLoop)
        {
            GBRelease(service->_runLoop);
            service->_runLoop = NULL;
        }
        
        DEBUG_ASSERT(service->_runLoop == NULL);
        
        service->_runLoop = runLoop;
        GBRetain(runLoop);
        
        return 1;
        
    }
    
    return 0;
}

BOOLEAN_RETURN uint8_t GBUPCServiceStart( GBUPCService* service)
{
    if( service
        && service->_callbacks.connectionRequestCallBack
        && service->_callbacks.dataCallBack
        && service->_callbacks.disconnectionCallBack
        && Internal_PrepareService(service))
    {
        DEBUG_ASSERT(service->_runLoop);
        
        return Internal_CreateListenners( service);
    }
    return 0;
}

BOOLEAN_RETURN uint8_t GBUPCServiceIsRunning( const GBUPCService* service)
{
    return service && service->_runLoop;
}



static BOOLEAN_RETURN uint8_t Internal_PrepareService( GBUPCService* service)
{
    DEBUG_ASSERT(service);
    
    /*
    if( service->portListen == UNINITIALIZED_FD)
        return 0;
    */
    if( service->_runLoop == NULL)
    {
        service->_runLoop = GBRunLoopInit();
    }
    
    return service->_runLoop != NULL;
}

static BOOLEAN_RETURN uint8_t Internal_CreateListenners( GBUPCService* service)
{
    DEBUG_ASSERT(service);
    DEBUG_ASSERT(service->_runLoop);
    
    if( service->_domainListener)
    {
        GBRunLoopRemoveSource(service->_runLoop, service->_domainListener);
        DEBUG_ASSERT(GBObjectGetRefCount(service->_domainListener) == 1);
        GBRelease(service->_domainListener);
        
        service->_domainListener = NULL;
    }
    if( service->_tcpListener)
    {
        GBRunLoopRemoveSource(service->_runLoop, service->_tcpListener);
        DEBUG_ASSERT(GBObjectGetRefCount(service->_tcpListener) == 1);
        GBRelease(service->_tcpListener);
        
        service->_tcpListener = NULL;
        
    }
    
    DEBUG_ASSERT(service->_domainListener == NULL);
    DEBUG_ASSERT(service->_tcpListener == NULL);
    
    uint8_t tcpOk = 0;
    uint8_t domainOk = 0;
    /* TCP Listener */
    if( service->portListen != UNINITIALIZED_FD)
    {
        service->_tcpListener = GBTCPSocketCreateListener( service->portListen , service->maxClients  , onListenner);
    }
    
    if(service->_tcpListener)
    {
        GBRunLoopSourceSetUserContext(service->_tcpListener, service);
        GBFDSourceShouldCloseOnDestruct(service->_tcpListener, 1);
        
        tcpOk = GBRunLoopAddSource(service->_runLoop, service->_tcpListener);
    }
    
    /* Domain Listener */
    const GBString* serviceFile = GBStringInitWithFormat("%s%s" , SERVICE_FOLDER , GBStringGetCStr(service->name));
    
    DEBUG_ASSERT(serviceFile);
    
    service->_domainListener = GBDomainCreateListener(GBStringGetCStr(serviceFile), service->maxClients , onListenner);
    GBRelease(serviceFile);
    
    if( service->_domainListener)
    {
        GBRunLoopSourceSetUserContext(service->_domainListener, service);
        GBFDSourceShouldCloseOnDestruct(service->_domainListener, 1);
        
        domainOk = GBRunLoopAddSource( service->_runLoop, service->_domainListener);
    }
    
    return (tcpOk || service->portListen == UNINITIALIZED_FD) && domainOk;
}


static BOOLEAN_RETURN uint8_t Internal_GBUPCServiceRespondToConnectionRequest( GBUPCService* service , GBRunLoopSource* socket)
{
    DEBUG_ASSERT(service->_clientProxies);
    
    GBFDSource* newClient = NULL;
    
    if (socket == service->_domainListener)
    {
        struct sockaddr_un cltAddr;
        socklen_t c = sizeof( struct sockaddr_un);
        
        newClient = GBDomainSocketAccept(socket , onClient,(struct sockaddr *) &cltAddr , &c);
        
    }
    else if (socket == service->_tcpListener)
    {
        struct sockaddr_in cltAddr;
        socklen_t c = sizeof( struct sockaddr_in);
        
        newClient = GBTCPSocketAccept(socket , onClient ,(struct sockaddr *) &cltAddr , &c);
    }
    else
    {
        DEBUG_ASSERT(0);
    }
    
    if (newClient)
    {
        GBFDSourceShouldCloseOnDestruct(newClient, 1);
        GBRunLoopSourceSetUserContext(newClient, service);
        
        GBUPCClientProxy *clientProxy = GBMalloc(sizeof(GBUPCClientProxy));
        clientProxy->_userContext = NULL;
        clientProxy->_socket = newClient;
        clientProxy->_canReadWrite = 0;
        clientProxy->_attachedService = service;
        
        if(service->_callbacks.connectionRequestCallBack(service, clientProxy)) // Service responded yes to connection request
        {
            GBUPCMessage resp;
            resp.mtype = UPCMessageCode_Accepted;
            resp.dataSize = 0;
            GBUPCMessageSendPrivate(newClient, &resp);
            
            if( GBRunLoopAddSource( service->_runLoop , newClient))
            {
		DEBUG_ASSERT( GBObjectGetRefCount( newClient) == 2);
                ListAddValue(service->_clientProxies, clientProxy);
                clientProxy->_canReadWrite = 1;
                return 1;
            }
        }
        else
        {
            GBFree(clientProxy);
            GBRelease(newClient);
            return 0;
        }
    }

    /**/
    /*
    else if (socket == service->_tcpListener)
    {
        struct sockaddr_in cltAddr;
        socklen_t c = sizeof( struct sockaddr_in);
        
        newClient = GBTCPSocketAccept(socket , onClient ,(struct sockaddr *) &cltAddr , &c);
        
        
        if (newClient)
        {
            GBFDSourceShouldCloseOnDestruct(newClient, 1);
            GBRunLoopSourceSetUserContext(newClient, service);
            
            GBUPCClientProxy *clientProxy = GBMalloc(sizeof(GBUPCClientProxy));
            clientProxy->_userContext = NULL;
            clientProxy->_socket = newClient;
            clientProxy->_canReadWrite = 0;
            
            if( service->_callbacks.connectionRequestCallBack(service, clientProxy))
            {
                return GBRunLoopAddSource( service->_runLoop , newClient);
            }
            else
            {
                GBRelease(newClient);
                return 0;
            }
            
            
        }
    }
     */
    DEBUG_ASSERT(0);
    return 0;
}

void GBUPCServiceSetUserContext(GBUPCService* service,  void* ptr)
{
    service->_userContext = ptr;
}
void* GBUPCServiceGetUserContext(const GBUPCService* service)
{
    return service->_userContext;
}

#ifdef DEBUG
static void Internal_Check( const GBUPCService* service)
{
    DEBUG_ASSERT(service);
    DEBUG_ASSERT(service->_clientProxies);
    DEBUG_ASSERT(ListGetSize(service->_clientProxies) < GBRunLoopGetNumSources(service->_runLoop));
}
#endif

BOOLEAN_RETURN uint8_t GBUPCServiceCloseAndRemoveClient( GBUPCService* service , const GBUPCClientProxy* client)
{
    if( service && client)
    {
        GBUPCClientProxy* proxy = GetProxyBySource(service, client->_socket);
        DEBUG_ASSERT(proxy == client);
        
        if(ListRemove(service->_clientProxies,client))
        {
            GBRunLoopRemoveSource(service->_runLoop, client->_socket);
            
            service->_callbacks.disconnectionCallBack(service , proxy , GBUPCDisconnectedByService);
            DEBUG_ASSERT( GBObjectGetRefCount(proxy->_socket) == 1);
	    DEBUG_LOG("before release : %zi \n" , GBObjectGetRefCount(proxy->_socket) );
	    GBRelease(proxy->_socket);
            GBFree(proxy);
            return 1;
        }
	else 
	{
		DEBUG_ASSERT(0);
	}
        
        
    }
    return 0;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* Send */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


BOOLEAN_RETURN uint8_t GBUPCServiceBroadcastMessage( GBUPCService* service ,  const GBUPCMessage* msg)
{
    if( service)
    {
        void* ptr = NULL;
        GBSize accum = 0;
        ListForEach(service->_clientProxies, ptr)
        {
            DEBUG_ASSERT(ptr);
            GBUPCClientProxy* clt = ptr;
            if( clt)
            {
                accum += GBUPCServiceSendMessage(service, clt, msg);
                
            }
        }
        
        return accum == ListGetSize(service->_clientProxies);
    }
    
    return 0;
}

BOOLEAN_RETURN uint8_t GBUPCServiceSendMessage( GBUPCService* service , const GBUPCClientProxy* client , const GBUPCMessage* message)
{
    DEBUG_ASSERT(service);
    if( service && client && client->_socket && client->_canReadWrite && message)
    {
        
        return GBUPCMessageSend( client->_socket , message );
        
    }
    return 0;
    
}

BOOLEAN_RETURN uint8_t GBUPCServiceBroadcastObject( GBUPCService* service , GBRef object , MsgType messageType )
{
    if( service)
    {
        
        GBBinCoder* coder = GBBinCoderInitWithRootObject(object);
        if( !object)
            return 0;
        
        GBUPCMessage msg;
        msg.dataSize = (uint32_t) GBBinCoderGetBufferSize(coder);
        msg.mtype = messageType;
        msg.data = GBBinCoderGetBuffer(coder);
        //memcpy(msg.data, GBBinCoderGetBuffer(coder), GBBinCoderGetBufferSize(coder) );

        void* ptr = NULL;
        GBSize accum = 0;
        ListForEach(service->_clientProxies, ptr)
        {
            DEBUG_ASSERT(ptr);
            GBUPCClientProxy* clt = ptr;
            if( clt)
            {
                accum += GBUPCMessageSend(clt->_socket , &msg);
                
            }
        }
        GBRelease(coder);
        return accum == ListGetSize(service->_clientProxies);
        
    }
    return 0;
}

BOOLEAN_RETURN uint8_t GBUPCServiceSendObject( GBUPCService* service , const GBUPCClientProxy* client ,GBRef object , MsgType messageType )
{
    DEBUG_ASSERT(service);
    if( service && client && client->_socket && client->_canReadWrite && object)
    {
        return GBUPCMessageSendObject( client->_socket, object, messageType);
    }
    return 0;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* Callbacks */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

static GBUPCClientProxy* GetProxyBySource( const GBUPCService* service , const GBRunLoopSource* source)
{
    DEBUG_ASSERT(service && service->_clientProxies);
    
    void* ptr = NULL;
    ListForEach(service->_clientProxies, ptr)
    {
        GBUPCClientProxy* clt = ptr;
        
        if( clt->_socket == source)
            return clt;
    }
    
    return NULL;
    
    
}

static void Internal_RemoveSource(GBUPCService* service, GBRunLoopSource* source)
{
    GBUPCClientProxy *proxy = GetProxyBySource(service , source);
    
    if( proxy)
    {
        ListRemove(service->_clientProxies, proxy);
    
        //DEBUG_ASSERT(proxy );
        proxy->_canReadWrite = 0;
        service->_callbacks.disconnectionCallBack(service ,proxy , GBUPCDisconnectedByClient);
        GBRelease(proxy->_socket);
        GBFree(proxy);

    }
    else
    {
        //("[Internal_RemoveSource] invalid proxy for source %p\n" , (void*) source);
        //DEBUG_ASSERT(0 && "[Internal_RemoveSource] invalid proxy for source");
    }
    
    GBRunLoopRemoveSource(service->_runLoop, source);
    //GBRelease(source);
    
}

/*
 TODO : This method is NOT REENTRANT (static variables) 
 */
static  void onClient( GBRunLoopSource* source , GBRunLoopSourceNotification notification)
{
    
    DEBUG_ASSERT(source);
    GBUPCService* service = GBRunLoopSourceGetUserContext(source);
    DEBUG_ASSERT(service);
    
#ifdef DEBUG
    Internal_Check(service);
#endif
    
    switch (notification)
    {
        case GBRunLoopSourceCanRead:
        {
            static GBUPCMessage msg;
            static uint8_t inBuffer[ UPCMessageMaxDataSize];
            
            memset(&msg, 0, sizeof(GBUPCMessage));
            memset(inBuffer, 0, sizeof(UPCMessageMaxDataSize));
            
            msg.data = inBuffer; // pointer ref

            if(GBUPCMessageRead( source ,&msg))
            {
                if( msg.mtype < UPCMessageCode_UserData)
                {
                    
                }
                else
                {
                    msg.mtype -= UPCMessageCode_UserData;
                    
                    
                    GBUPCClientProxy *proxy = GetProxyBySource(service , source);
                    DEBUG_ASSERT(proxy && proxy->_canReadWrite);
                    service->_callbacks.dataCallBack(service , proxy ,&msg);
                }
            }
            else
            {
                Internal_RemoveSource(service, source);
            }
            
        }
        break;
            
        case GBRunLoopSourceError:
        case GBRunLoopSourceDisconnected:
        {
            Internal_RemoveSource(service, source);
            
            
        }
            break;
            
        default:
            //("Client sok notif : %i\n" ,notification);
            DEBUG_ASSERT(0 && "[GBUPC Service] Unhandled Socket notification"); // to inspect
            break;
            
    }
}
static  void onListenner( GBRunLoopSource* source , GBRunLoopSourceNotification notification)
{
    
    DEBUG_ASSERT(source);
    GBUPCService* service = GBRunLoopSourceGetUserContext(source);
    DEBUG_ASSERT(service);
    DEBUG_ASSERT(source == service->_tcpListener || source == service->_domainListener);

#ifdef DEBUG
    Internal_Check(service);
#endif
    
    if( notification == GBRunLoopSourceCanRead)
    {
        Internal_GBUPCServiceRespondToConnectionRequest(service , source);
        
    }
    else
    {
        DEBUG_ASSERT(0); // should not occur...
    }
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* GBUPCClientProxy */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


GBUPCService* GBUPCClientProxyGetService( GBUPCClientProxy* proxy)
{
    if( proxy)
    {
        return proxy->_attachedService;
    }
    return NULL;
}
void GBUPCClientProxySetUserContext( GBUPCClientProxy* proxy , void* ptr)
{
    if( proxy )
    {
        proxy->_userContext = ptr;
    }
}
void* GBUPCClientProxyGetUserContext( const GBUPCClientProxy* proxy)
{
    if( proxy )
    {
        return proxy->_userContext;
    }
    return NULL;
}

BOOLEAN_RETURN uint8_t GBUPCServiceGetClientCredentials( GBUPCService* service , const GBUPCClientProxy* client, GBUPCClientCredentials* credentials)
{
    if(credentials && service && client && client->_attachedService == service)
    {
        
        
        if( getpeereid( GBFDSourceGetFileDescriptor( client->_socket ), &credentials->clientUID, &credentials->clientGID) == 0)
        {
            credentials->clientPID = 0;
            return 1;
        }
        else
        {
            PERROR("GBUPCServiceGetClientCredentials.getpeereid");
        }
    }
    return 0;
}
