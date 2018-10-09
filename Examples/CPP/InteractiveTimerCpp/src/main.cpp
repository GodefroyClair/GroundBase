//
//  main.c
//  InteractiveTimerC
//
//  Created by Manuel Deneu on 17/01/2018.
//  Copyright © 2018 Manuel Deneu. All rights reserved.
//

#include <stdio.h>
#include <unistd.h> // STDIN_FILENO
#include <string>
#include <GBTimer.hpp>
#include <GBRunLoop.hpp>
#include <GBFDSource.hpp>

#define BUFFER_MAX_SIZE (size_t) 128


int main()
{
    
    
    GB::RunLoop runloop;
    
    GB::Timer timer1([]()
    {
        static int count = 0;
        printf("On time %i\n" , count++);
        
    } , 1000);
    
    
    GB::FDSource inputSource(STDIN_FILENO , [&inputSource , &runloop ,&timer1]( GB::FDSource::Notification notif)
    {
        if( notif == GB::FDSource::Notification::CanRead)
        {
            static char buffer [ BUFFER_MAX_SIZE ] = {'0'};
            std::size_t readSize = inputSource.read( &buffer, BUFFER_MAX_SIZE);
            buffer[readSize-1] = 0;
            
            const std::string cmd = buffer;
            
            if( cmd == "quit")
            {
                runloop.stop();
            }
            else if( cmd == "pause")
            {
                printf("%s timer\n" , timer1.isActive()?"Pause":"Resume" );
                timer1.setActive( !timer1.isActive() );
            }
        }
    });
    
    
    runloop.addSource(inputSource);
    runloop.addSource(timer1);
    printf("Interactive Timer Test : type 'quit' to return, pause to pause/resume the timer \n");
    runloop.run();
    printf("RunLoop returned \n");
    
    return 0;
}
