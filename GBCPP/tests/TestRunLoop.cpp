//
//  TestBase.cpp
//  GroundBaseCPP
//
//  Created by Manuel Deneu on 03/11/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include "TestBase.hpp"

#include <string.h>
#include <unistd.h>
#include <iostream>
#include <GroundBase.hpp>
#include <GBObject.hpp>
#include <GBString.hpp>
#include <GBDictionary.hpp>
#include <GBArray.hpp>
#include <GBRunLoop.hpp>
#include <GBTimer.hpp>
#include <GBFDSource.hpp>

bool testRunLoop()
{
    GB::RunLoop rl;

    assert(rl.getNumTimers() == 0);
    assert(rl.getNumSources() == 0);
    assert(rl.getNumFDSources() == 0);
    
    
    
    GB::Timer t1([ &t1 ]()
                 {
                     assert(t1.isActive());
                     
                     printf("On time TEst\n");
                 } , 200);
    
    
    assert(t1.getInterval() == 200);
    
    GBTimer* timer = GBTimerInit([] (GBRunLoopSource*  , GBRunLoopSourceNotification )
                                 {
                                     printf("On time \n");
                                 });
    
    GBTimerSetIntervalMS(timer, 500);
    
    rl.async([&rl]
             {
                 printf("Async call 1 \n");
                 
                 rl.after([&rl]
                          {
                              printf("Async call 2\n");
                              printf("Time to stop\n");
                              rl.stop();
                          } , 1500);
                 
                 
             });
    
    rl.addSource(timer);
    rl.addSource(t1);
    
    
    //assert(rl.containsSource(t1));
    rl.run();
    
    GBRelease(timer);
    
    return true;
}


bool testFDSource()
{
    GB::FDSource inSource( STDIN_FILENO);
    
    
    printf("Type quit (+enter) to exit \n");
    
    inSource.sourceCallback = [&inSource] ( GB::FDSource::Notification notif )
    {
        if( notif == GB::FDSource::Notification::CanRead)
        {
            static char buffer[128] = {0};
            const size_t readBytes = inSource.read(buffer, 128);
            buffer[readBytes-1] = 0;
            printf("Did Read %zi bytes : '%s'\n" ,readBytes, buffer);
            
            if( strcmp(buffer, "quit") == 0)
            {
                inSource.getRunLoop().stop();
                
            }
        }
        
        
    };
     
    GB::RunLoop rl;
    
    GB::Timer timer([]()
    {
        printf("On time\n");
    } , 1000);
    
    rl.addSource(inSource);
    rl.addSource(timer);
    
    rl.run();
    return true;
}
