//
//  testUPCBase.c
//  UnitTests
//
//  Created by Manuel Deneu on 15/10/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include <stdio.h>
#include "testUPCBase.h"
#include <GBUPCClient.h>
#include <GBUPCService.h>

void testUPCClient()
{
    printf("----- test GBUPC Client Base ----- \n");
    
    
    assert(GBUPCClientConnectToEndPoint(NULL, NULL) == 0);
    assert(GBUPCClientDisconnect(NULL) ==0);

    assert(GBUPCClientSetRunLoop( NULL , NULL) == 0);
    
    
    assert(GBUPCClientGetUserContext(NULL) == NULL);
    
    assert(GBUPCClientSendMessage( NULL , NULL ) == 0);
    assert(GBUPCClientSendObject( NULL , NULL , 1 ) == 0);
    
    /**/
    
    GBUPCClient* client = NULL;
    
    GBUPCClientCallBacks c;
    c.dataCallBack = NULL;
    c.notificationCallback = NULL;
    client = GBUPCClientInit(c);
    
    assert(client);
    
    assert(GBUPCClientIsConnected(client) == 0);
    assert(GBUPCClientConnectToEndPoint(client, NULL) == 0);
    assert(GBUPCClientIsConnected(client) == 0);
    assert(GBUPCClientDisconnect(client) ==0);
    
    assert(GBUPCClientGetRunLoop( client) == NULL);
    assert(GBUPCClientSetRunLoop( client , NULL) == 0);
    assert(GBUPCClientGetRunLoop( client) == NULL);
    
    assert(GBUPCClientRun( client) == 0);
    
    //assert(GBUPCClientIsRunning(client) == 0); // deprecated call
    assert(GBUPCClientGetUserContext(client) == NULL);
    
    assert(GBUPCClientSendMessage( client , NULL ) == 0);
    assert(GBUPCClientSendObject( client , NULL , 1 ) == 0);
    
    
    GBRelease(client);
    
    
    
}

void testUPCService()
{
    printf("----- test GBUPC Service Base ----- \n");
    
    {
        assert( GBUPCServiceInitWithName(NULL) == NULL);
        
        const GBString* invalidName = GBStringInitWithCStr("");
        
        assert( GBUPCServiceInitWithName(invalidName) == NULL);
        
        
        
        GBRelease(invalidName);
    }
    
    {
        const GBString* serviceName = GBStringInitWithCStr("myService");
        GBUPCService* service = GBUPCServiceInitWithName( serviceName);
        assert(service);
        assert( GBStringEquals( GBUPCServiceGetName(service) , serviceName) );
        
        assert(GBUPCServiceIsRunning(service) == 0);
        assert(GBUPCServiceGetNumClients(service) == 0);
        
        assert(GBUPCServiceSetListeningPort(service, -1) == 0);
        
        assert(GBUPCServiceStart(service)==0);
        assert(GBUPCServiceCloseAndRemoveClient(service,NULL) == 0);
        
        
        GBRelease(service);
        GBRelease(serviceName);
    }
}

