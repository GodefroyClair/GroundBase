//
//  mainServer.cpp
//  UPCDemoCPP
//
//  Created by Manuel Deneu on 08/12/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include <thread>
#include <iostream>
#include <GBUPCService.hpp>

#include "../../../Commons/DemoUPC.h"

static GB::UPC::Service* _service = nullptr;


class MyServiceDelegate : public GB::UPC::ServiceDelegate
{
    void didReceiveData( GB::UPC::Service &service , const GB::UPC::ClientProxy &client , const GB::UPC::Message & msg )
    {
        
        GB::Variant datas;
        if( msg.decodeRootObject(datas) )
        {
            
            if( datas.isConvertibleToDictionary())
            {
                const GB::Variant v1 = datas.getDictionary().at(VALUE_1_KEY);
                const GB::Variant v2 = datas.getDictionary().at(VALUE_2_KEY);
                
                if( v1.isConvertibleToNumber() && v2.isConvertibleToNumber() )
                {
                    printf("Will sum %f and %f\n" , v1.toFloat() , v2.toFloat() );
                    
                    GB::Variant response(
                                         {
                                             { RESPONSE_KEY , GB::Variant( RESPONSE_OK )},
                                             { DATA_KEY , GB::Variant( (float) v1.toFloat() + v2.toFloat() )}
                                         });
                    
                    service.sendObject(client, response, DemoUPCCompute1_Response);
                    
                    
                    return;
                }
                
            }
            printf("Error , invalid request \n");
            GB::Variant response(
            {
                { RESPONSE_KEY , GB::Variant( RESPONSE_ERROR )}
            });
            
            service.sendObject(client, response, DemoUPCCompute1_Response);
        }
        
    }
    
    bool connectionRequest( GB::UPC::Service & , const GB::UPC::ClientProxy &client)
    {
        GBUPCEndPoint clientEndPoint{};
        
        if( client.getEndPoint(clientEndPoint))
        {
            std::cout << "Got connection request :";
            if( clientEndPoint.connectionType == GBUPCTypeDistant)
            {
                std::cout << " IP client " << GBStringGetCStr( clientEndPoint.client.distant.address ) << " \n";
            }
            else if( clientEndPoint.connectionType == GBUPCTypeLocal)
            {
                std::cout << " Domain client \n";
            }
        
            GBUPCEndPointRelease(&clientEndPoint);
            
            return true;
        }
        
        GBUPCEndPointRelease(&clientEndPoint);

        return false;
    }
    
    void clientDisconnected( GB::UPC::Service & , const GB::UPC::ClientProxy & , GBUPCDisconnectionReason)
    {
        std::cout << "Client disconnected \n";
    }
};


static void sigTermHandler(int signo)
{
    _service->getRunLoop().stop();
}

int main(int argc, const char * argv[])
{
    
    {
        GB::UPC::Service service( DEMO_UPC_SERVICE_NAME );
        _service = &service;
        
        if (signal(SIGINT, sigTermHandler) == SIG_ERR)
        {
            return 4;
        }
        
        if (signal(SIGTERM, sigTermHandler) == SIG_ERR)
        {
            return 4;
        }
        
        MyServiceDelegate delegate;
        service.setDelegate( &delegate);
        
        service.setListeningPort(DEMO_UPC_SERVICE_PORT);
        
        if( service.start() )
        {
            std::cout << "Start ok, type ctrl+C to stop \n";
            service.getRunLoop().run();
            std::cout << "Service stopped \n";
            
        }
        else
        {
            std::cout << "Error starting Service \n";
        }
    }
    
    GB::Introspection(true);
    
    
    return 0;
}
