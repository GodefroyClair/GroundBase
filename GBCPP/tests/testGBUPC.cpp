//
//  testGBUPC.cpp
//  UnitTestsCPP
//
//  Created by Manuel Deneu on 07/12/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include <GBUPCClient.hpp>
#include <GBUPCService.hpp>
#include "TestBase.hpp"

bool testUPCClient()
{
    GB::UPC::Client client;
    
    assert(client.isConnected() == false);
    
    
    /* Connection tries */
    
    assert(client.connect("") == false);
    assert(client.isConnected() == false);
    
    assert(client.connect("lol") == false);
    assert(client.isConnected() == false);
    
    assert(client.connect("" , 1) == false);
    assert(client.isConnected() == false);
    
    assert(client.connect("lol" , 1) == false);
    assert(client.isConnected() == false);
    
    assert(client.connect("127.0.0.1" , -1) == false);
    assert(client.isConnected() == false);
    
    assert(client.connect("127.0.0.1" , 0) == false);
    assert(client.isConnected() == false);
    
    assert(client.connect("127.0.0.1" , 12374) == false); // this one is legit, but there is no server on this port? (Maybe?)
    assert(client.isConnected() == false);
    
    /* Send messages */
    
    assert(client.sendMessage(NULL) == false);
    
    GBUPCMessage msg;
    GBUPCMessageInit(&msg);
    assert(client.sendMessage( &msg) == false);
    
    GBRef invalidObj = nullptr;
    
    assert(client.sendObject(invalidObj, 0) == false);
    
    return true;
    
}

bool testUPCService()
{
    printf("----- Test GB::UPC::Service ----- \n");
    
    {
        GB::UPC::Service service("");
        assert(service.isValid() == false);
        assert(service.isRunning() == false);
    }
    const std::string serviceName = "serviceName";
     GB::UPC::Service service( serviceName );
    assert(service.isValid());
    assert(service.getName() == serviceName);
    
    assert(service.isRunning() == false);
    assert(service.getNumClients() == 0);
    assert(service.setListeningPort(-1)== false);
    assert(service.start() == false );
    
    
    return true;
    
}
