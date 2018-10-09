//
//  testGBUPCService.c
//  GroundBase
//
//  Created by Manuel Deneu on 26/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include "testGBUPCService.h"


#include <stdio.h>
#include <GBUPCService.h>


static void serviceDidReceiveData( GBUPCService* service, const GBUPCClientProxy* client, const GBUPCMessage* data  )
{
    UNUSED_PARAMETER(service);
    UNUSED_PARAMETER(client);
    UNUSED_PARAMETER(data);
    printf("Received Data\n");
}
static BOOLEAN_RETURN uint8_t serviceClientConnectionRequest( GBUPCService* service ,GBUPCClientProxy* client )
{
    UNUSED_PARAMETER(service);
    UNUSED_PARAMETER(client);
    
    printf("Connection request from\n ");
    return 0;
}
static void serviceClientDisconnected( GBUPCService* service , const GBUPCClientProxy* client , GBUPCDisconnectionReason reason )
{
    UNUSED_PARAMETER(service);
    UNUSED_PARAMETER(client);
    UNUSED_PARAMETER(reason);
    printf("Client Disconnected\n ");
}


void testGBUPCService()
{
    printf("----- test GBUPCService ----- \n");
    
    assert(GBUPCServiceInitWithName(NULL) == NULL);
    
    const GBString* name = GBStringInitWithCStr("service.name");
    
    GBUPCService* service = GBUPCServiceInitWithName( name );
    assert(service);
    assert(GBUPCServiceIsRunning(service) == 0);
    assert( GBStringEquals( GBUPCServiceGetName(service) , name ));
    
    assert(GBObjectGetRefCount(name) == 2);
    GBRelease(name);
    assert(GBObjectGetRefCount(name) == 1);
    
    assert(GBUPCServiceGetRunLoop(service) == NULL);
    
    
    assert(GBUPCServiceStart(service) == 0); // callbacks not set, must fail 
    
    GBUPCServiceCallBacks methods;
    methods.connectionRequestCallBack = serviceClientConnectionRequest;
    methods.dataCallBack = serviceDidReceiveData;
    methods.disconnectionCallBack = serviceClientDisconnected;
    
    GBUPCServiceSetCallBacks(service, methods);
    
    assert(GBUPCServiceStart(service) == 0); // must fail because listenning port not set
    
    assert( GBUPCServiceSetListeningPort(service, 9999) );

    assert(GBUPCServiceStart(service));
    
    assert( GBUPCServiceSetListeningPort(service, 9999) == 0 );
    assert(GBUPCServiceIsRunning(service));
    assert(GBUPCServiceGetRunLoop(service) ); // GBUPCServiceStart has created a new runloop
    
    GBRunLoop* rl = GBUPCServiceGetRunLoop(service);
    
    GBRunLoopRun(rl);
    
    
    GBRelease(service);
    
}
