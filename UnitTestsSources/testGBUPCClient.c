//
//  testGBUPCClient.c
//  GroundBaseNew
//
//  Created by Manuel Deneu on 26/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include "testGBUPCClient.h"

#include <string.h>
#include <stdio.h>
#include <GBUPCClient.h>
#include <GBDictionary.h>
#include <GBNumber.h>

static void clientDidReceiveData( GBUPCClient* client, const GBUPCMessage* data  )
{
    assert(client);
    
    const GBBinCoder* decoder = GBUPCMessageReadObject(data);
    assert(decoder);
    
    printf("Received object , type %i \n" , data->mtype);
    GBRelease(decoder);
    /*
    printf("Client got data '%s' \n" , (const char*)data->data);
    
    GBUPCClientSendMessage(client, data);
     */
    
}

static void clientNotification ( GBUPCClient* client , GBUPCNotification notification )
{
    assert(client);
    
    if( notification == GBUPCNotificationConnected)
    {
        printf("Client Connected to Service\n");
        
        
        GBDictionary* dict = GBDictionaryInit();
        
        const GBString* key1 = GBSTR("Key1");
        const GBNumber* val1 = GBNumberInitWithFloat(0.f);
        
        GBDictionaryAddValueForKey(dict,val1 , key1);
        GBRelease(val1);
        
        const GBString* key2 = GBSTR("Key2");
        const GBNumber* val2 = GBNumberInitWithFloat(200.f);
        
        GBDictionaryAddValueForKey(dict,val2 , key2);
        GBRelease(val2);
        
        assert(GBUPCClientSendObject(client, dict, 12));
        GBRelease(dict);
        
        
    }
    else if( notification == GBUPCNotificationDisconnected)
    {
        printf("Service disconnected \n");
        
        GBRunLoopStop( GBUPCClientGetRunLoop(client));
        
        
    }
    else
    {
        printf("Client notification %i \n" , notification);
    }
}

void testGBUPCClient()
{
    printf("----- test GBUPCClient ----- \n");
    GBUPCClientCallBacks callbacks;
    callbacks.dataCallBack = clientDidReceiveData;
    callbacks.notificationCallback = clientNotification;
    

    GBUPCClient* client = GBUPCClientInit(callbacks);
    
    assert(client);
    
    assert(GBUPCClientGetRunLoop(client) == NULL);
    
    GBUPCEndPoint endPoint;
    GBUPCEndPointInit(&endPoint);
    endPoint.connectionType  = GBUPCTypeLocal;
    endPoint.client.local.name = GBStringInitWithCStr("service.name");
    
    assert(GBUPCEndPointIsValid(&endPoint));
    
    if (GBUPCClientConnectToEndPoint(client, &endPoint))
    //if(GBUPCClientConnectToTCP( client , "127.0.0.1" , 9999))
    {
        //printf("Connection ok \n");
    }
    else
    {
        printf("Error : unable to connect to Service \n");
    }
    
    GBUPCEndPointRelease(&endPoint);
    
    
    GBRelease(client);
}
