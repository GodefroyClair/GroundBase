//
//  main.c
//  TimerC
//
//  Created by Manuel Deneu on 17/01/2018.
//  Copyright © 2018 Manuel Deneu. All rights reserved.
//

#include <stdio.h>
#include <GBRunLoop.hpp>
#include <GBTimer.hpp>

/*
 A simple program that run 10 times a 1 second timer.
 For a more complex example see 'InteractiveTimerCpp' project.
 */


int main()
{
    
    GB::RunLoop runloop;
    
    GB::Timer timer1( [&runloop]()
    {
        static int count = 0;
        
        printf("On time %i\n" , count);
        if( ++count == 10)
        {
            runloop.stop();
        }
    } , 1000);
    
    
    runloop.addSource( timer1);
    
    runloop.run();
    
    return 0;
}
