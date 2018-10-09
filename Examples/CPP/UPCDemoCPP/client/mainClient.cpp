//
//  main.cpp
//  UPCDemoCPP
//
//  Created by Manuel Deneu on 08/12/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include <GBUPCClient.hpp>
#include "../../../Commons/DemoUPC.h"

class ClientDelegate  : public GB::UPC::ClientDelegate
{
    void didReceiveData( GB::UPC::Client &client , const GB::UPC::Message &message)
    {
        std::cout << "Received Data \n";
        
        GB::Variant datas;
        
        if( message.decodeRootObject(datas))
        {
            
            const GB::Variant response = datas.getDictionary().at(RESPONSE_KEY);
            
            if( response.getString() == RESPONSE_OK)
            {
                const GB::Variant data = datas.getDictionary().at(DATA_KEY);
                printf("Response status '%s' Computed sum : %f\n" , response.getString().c_str(), data.toFloat() );
            }
            else
            {
                printf("Error \n");
            }
        }
        
        client.getRunLoop().stop();
    }
    
    void receivedNotification( GB::UPC::Client& client , GBUPCNotification notif)
    {
        switch (notif)
        {
            case GBUPCNotificationDisconnected:
                std::cout << "Connection to Service closed, will stop \n";
                client.getRunLoop().stop();
                
                break;
                
            case GBUPCNotificationConnected:
            {
                std::cout << "Connected \n";
                
                GB::Variant dict(
                                 /*
                {
                    { VALUE_1_KEY , GB::Variant("40") },
                    { VALUE_2_KEY , GB::Variant("2") }
                });
                                  */
                {
                 { VALUE_1_KEY , GB::Variant(40) },
                 { VALUE_2_KEY , GB::Variant(2) }
                 });
                
                client.sendObject(dict, DemoUPCCompute1);
                
                break;
            }
                
            default:
                std::cout << "Got other notif " << notif << "\n";
                break;
        }
        
    }
};

int main(int argc, const char * argv[])
{
    GB::enableInvalidReleaseDebug(true);
    GB::UPC::Client client;
    ClientDelegate delegate;
    client.setDelegate( &delegate);
    
    if( client.connect(DEMO_UPC_SERVICE_NAME ))
    //if( client.connect("127.0.0.1", DEMO_UPC_SERVICE_PORT))
    {
        std::cout << "Connection to " << DEMO_UPC_SERVICE_NAME << " ok \n";
        
        client.run(); // will block
    }
    else
    {
        std::cout << "Connection Error to " << DEMO_UPC_SERVICE_NAME << "\n";
    }
    
    return 0;
}
