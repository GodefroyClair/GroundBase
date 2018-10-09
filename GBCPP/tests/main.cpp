//
//  main.cpp
//  GrounBaseCPP
//
//  Created by Manuel Deneu on 27/10/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//


#include <unistd.h>
#include <GBObject.hpp>
#include "TestBase.hpp"


int main(int , const char ** )
{

    GB::enableInvalidReleaseDebug(true);
    

    testRuntime();
    testString();
    testDictionary();
    testArray();
    testLambdaString();
    
    testVariant();
    testJSON();
    
    testRunLoop();
    
    //testFDSource();
    
    testSharedMemory();
     
    testPropertyList();
    
    testUPCClient();
    testUPCService();
    
    GB::Introspection(true);
    
    sleep(4);
    return 0;
}
