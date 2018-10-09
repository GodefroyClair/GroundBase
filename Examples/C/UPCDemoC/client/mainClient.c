//
//  mainClient.c
//  UPCDemoC
//
//  Created by Manuel Deneu on 08/12/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//
#include <GBDictionary.h>
#include <GBUPCClient.h>
#include "DemoUPC.h"

static void onData( GBUPCClient* client,  const GBUPCMessage* data  )
{
    printf("Received datas\n");
    
    GBBinCoder* decoder = GBUPCMessageReadObject(data);
    
    if( decoder)
    {
        const GBDictionary* results = GBBinCoderDecodeRoot(decoder);
        
        GBObjectClassRef response = GBDictionaryGetValueForKey(results, GBSTR(RESPONSE_KEY));
        GBObjectClassRef datas = GBDictionaryGetValueForKey(results, GBSTR(DATA_KEY));
        
        if( response && datas && IsKindOfClass(datas, GBNumberClass) && GBStringEqualsCStr( (const GBString*) response, RESPONSE_OK) )
        {
            printf("Result is %f \n" , GBNumberGetFloat( (const GBNumber*) datas));
            
            
        }
        else
        {
            printf("Service returned an error. \n");
        }
        
        GBRelease(results);
        GBRelease(decoder);
        
    }
    
    GBRunLoopStop( GBUPCClientGetRunLoop(client) );
}

static void onNotification( GBUPCClient* client , GBUPCNotification notification )
{
    if ( notification == GBUPCNotificationConnected)
    {
        GBDictionary* parameters = GBDictionaryInit();
        
        GBNumber* v1 = GBNumberInitWithFloat(-100.f);
        GBDictionaryAddValueForKey(parameters, v1, GBSTR(VALUE_1_KEY));
        GBRelease(v1);
        
        
        GBNumber* v2 = GBNumberInitWithFloat(123.f);
        GBDictionaryAddValueForKey(parameters, v2, GBSTR(VALUE_2_KEY));
        GBRelease(v2);
        /*
        GBString* v2 = GBStringInitWithCStr("Lol");
        GBDictionaryAddValueForKey(parameters, v2, GBSTR(VALUE_2_KEY));
        GBRelease(v2);
        */
        
        GBUPCClientSendObject(client, parameters, DemoUPCCompute1);
        GBRelease(parameters);
    }
}

int main(int argc, const char * argv[])
{
    GBRuntimeEnableInvalidReleaseDebug(1);
    
    GBUPCClientCallBacks callbacks;
    
    callbacks.dataCallBack = onData;
    callbacks.notificationCallback = onNotification;
    
    GBUPCClient* client = GBUPCClientInit(callbacks);
    
    
    if( GBUPCClientConnectToLocalEndPoint(client, DEMO_UPC_SERVICE_NAME) )
    {
        GBUPCClientRun(client);
    }
    else
    {
        printf("Error: unable to connect to service '%s' \n", DEMO_UPC_SERVICE_NAME);
    }
    
    
    GBRelease(client);
    return 0;
}
