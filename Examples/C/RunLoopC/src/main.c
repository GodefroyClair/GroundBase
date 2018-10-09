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

#include <GBRunLoop.h>
#include <GBFDSource.h>
#include <GBThread.h>



#define BUFFER_MAX_SIZE (size_t) 128

typedef struct
{
    GBRunLoop* runloop1;
    GBThread* threadRunloop2;
    GBRunLoop* runloop2;
    
    GBFDSource* inputSource;
    
} RunLoopContext;


static void switchRunLoops(GBRunLoop* runLoop , void* data)
{
    RunLoopContext* context = data;
    assert(context);
    
    GBRunLoop* currentRL = GBRunLoopSourceGetRunLoop(context->inputSource);
    GBRunLoop* destRL    = currentRL == context->runloop1? context->runloop2 : context->runloop1;
    
    assert(currentRL != destRL );
    
    printf("Async call to switch from %s to %s\n" , currentRL == context->runloop1 ? "runloop1" : "runloop2" , destRL == context->runloop1 ? "runloop1" : "runloop2");
    
    GBRunLoopRemoveSource(currentRL, context->inputSource);
    
    GBRunLoopAddSource( destRL, context->inputSource );
}


static void inputCallback( GBRunLoopSource* source , GBRunLoopSourceNotification notification)
{
    
    // retreive context instance set in 'main()' with GBRunLoopSourceSetUserContext
    RunLoopContext* context = GBRunLoopSourceGetUserContext( source );
    
    if (notification == GBRunLoopSourceCanRead)
    {
        static char buffer[ BUFFER_MAX_SIZE ] = { 0 };
        
        const GBSize retRead = GBFDSourceRead( source, &buffer, BUFFER_MAX_SIZE);
        
        if( retRead)
        {
            buffer[ retRead - 1 ] = 0;
            
            printf("Input '%s' from %s\n" , buffer ,GBThreadCalledFromThis( context->threadRunloop2)?"runloop2" : "runloop1" );
            if( strcmp(buffer, "quit") == 0)
            {
                // stop both runloops
                GBRunLoopStop(context->runloop1);
                GBRunLoopStop(context->runloop2);
            }
            else if( strcmp(buffer, "t") == 0)
            {
                
                // check the runloop we're running on.
                
                GBRunLoop* currentRunloop = GBRunLoopSourceGetRunLoop( source );
                
                assert( currentRunloop);
                assert( currentRunloop == context->runloop1 || currentRunloop == context->runloop2 );
                
                
                if (currentRunloop == context->runloop1)
                {
                    assert(GBThreadCalledFromThis( context->threadRunloop2) == 0);
                    
                    //printf("Time to transfert input from runloop1 to runloop2 \n");
                    
                    GBRunLoopDispatchAsync(context->runloop2, switchRunLoops, context);

                }
                else if (currentRunloop == context->runloop2)
                {
                    assert(GBThreadCalledFromThis( context->threadRunloop2));
                    GBRunLoopDispatchAsync(context->runloop1, switchRunLoops, context);

                }
            }
            
        }
        
    }
    
}


static void onThread2( GBThread *self)
{
    RunLoopContext* context = GBThreadGetUserContext( self);
    
    GBRunLoopRun( context->runloop2);
}

int main()
{
    RunLoopContext context;
    context.runloop1 = GBRunLoopInit();
    context.runloop2 = GBRunLoopInit();
    
    context.threadRunloop2 = GBThreadInit();
    
    // we use thread's User context to pass around a reference to RunLoopContext instance, and thus avoid using static variables.
    GBThreadSetUserContext( context.threadRunloop2, &context);
    
    GBThreadSetMain( context.threadRunloop2 , onThread2 );
    
    context.inputSource = GBFDSourceInitWithFD(STDIN_FILENO, inputCallback );
    
    // we use source's User context to pass around a reference to RunLoopContext instance, and thus avoid using static variables.
    GBRunLoopSourceSetUserContext( context.inputSource , &context);
    
    // First we process inputSource on runloop1
    GBRunLoopAddSource( context.runloop1, context.inputSource );
    
    
    printf("Interactive test : type 'quit' to return, 't' to switch runloops.\n");
    
    // start runloop2's thread
    GBThreadStart( context.threadRunloop2 );
    
    // run runloop1 on main thread.
    GBRunLoopRun(context.runloop1);
    
    GBRelease( context.inputSource );
    GBRelease( context.threadRunloop2 );
    GBRelease( context.runloop1 );
    GBRelease( context.runloop2 );
    return 0;
}
