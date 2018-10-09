//
//  main.c
//  RunLoopC
//
//  Created by Manuel Deneu on 17/01/2018.
//  Copyright © 2018 Manuel Deneu. All rights reserved.
//

#include <stdio.h>
#include <unistd.h> // STDIN_FILENO
#include <string.h>
#include <assert.h>

#include <GBFDSource.hpp>
#include <GBRunLoop.hpp>
#include <thread>


#define BUFFER_MAX_SIZE (size_t) 128

class Controler
{
public:
    
    
    Controler():
    inputSource( STDIN_FILENO , std::bind(&Controler::onInput, this , std::placeholders::_1))
    {}
    
    bool start()
    {
        
        runloop2Thread = std::thread([this]()
        {
            runloop2.run();
            printf("Runloop2 returned\n");
        });
        
        runloop1.addSource( inputSource);
        
        if(runloop1.run())
        {
            runloop2Thread.join();
            return true;
        }
        return false;
    }
private:
    
    void onInput(GB::FDSource::Notification notification)
    {
        if (notification == GB::FDSource::Notification::CanRead)
        {
            static char buffer[ BUFFER_MAX_SIZE ] = { 0 };
            
            const size_t retRead = inputSource.read(&buffer, BUFFER_MAX_SIZE);
            
            if( retRead)
            {
                buffer[ retRead - 1 ] = 0;
                
                //printf("Input '%s' from %s\n" , buffer ,GBThreadCalledFromThis( context->threadRunloop2)?"runloop2" : "runloop1" );
                printf("Input '%s' from %s\n" , buffer , inputSource.getRunLoop() == runloop2 ? "runloop2" : "runloop1" );
                
                if( strcmp(buffer, "quit") == 0)
                {
                    stop();
                }
                else if( strcmp(buffer, "t") == 0)
                {
                    if( inputSource.getRunLoop() == runloop1)
                    {
                        printf("Is on runloop1\n");
                    }
                    else if( inputSource.getRunLoop() == runloop2)
                    {
                        printf("Is on runloop2\n");
                    }
                    
                    switchRunLoops();
                }
                
            }
        }
    }
    
    void switchRunLoops()
    {
        
        if( inputSource.getRunLoop() == runloop1)
        {
            runloop2.addSource(inputSource);
        }
        else
        {
            assert(inputSource.getRunLoop() == runloop2);
            runloop1.addSource(inputSource);
        }
    }
    
    void stop()
    {
        runloop1.stop();
        runloop2.stop();
    }
    
    std::thread runloop2Thread;
    GB::FDSource inputSource;
    GB::RunLoop runloop1;
    GB::RunLoop runloop2;
};


int main()
{
    
    Controler controller;
    
    printf("Interactive test : type 'quit' to return, 't' to switch runloops.\n");
    controller.start();
    printf("Runloop1 returned\n");
    return 0;
}
