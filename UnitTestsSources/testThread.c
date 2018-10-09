/*
 * Copyright (c) 2016 FlyLab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
//
//  testThread.c
//  GroundBase
//
//  Created by Manuel Deneu on 20/09/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <GroundBase.h>
#include <GBString.h>
#include <GBThread.h>
#include "testThread.h"

static void mainThread(void);
static void* secondThread(void* data);

static GBObject* object = NULL;



void testThread()
{
    printf("----- Test Thread ----\n");
    
    object = GBStringInitWithFormat("Hello%i" , 0);
    
    pthread_t client;
    
    if (pthread_create(&client, NULL, secondThread, NULL))
    {
        perror("pthread_create");
        return;
    }
    
    
    mainThread();
    
    sleep(1);
    pthread_join(client, NULL);
    
    GBRelease(object);
}

static void mainThread()
{
    printf("--- Main Thread --\n");
    
    int value = 0;
    while (value < 100)
    {
        
        if ( GBObjectTryLock(object))
        {
            printf("[Main Thread] got lock '%s' \n", GBStringGetCStr(object));
            
            value++;
            
            GBStringSetContentWithFormat(object, "Hello%i" , value);
            
            GBObjectUnlock(object);
        }
        usleep(1000*10);
    }
    
}

static void* secondThread(void* data)
{
    UNUSED_PARAMETER(data);
    printf("--- Second Thread --\n");
    
    int value = 0;
    while (value < 100)
    {
        
        if ( GBObjectTryLock(object))
        {
            printf("[Second Thread] got lock '%s' \n", GBStringGetCStr(object));
            
            value++;
            GBStringSetContentWithFormat(object, "Hello%i" , value);
            
            GBObjectUnlock(object);
        }
        usleep(1000*10);
    }
    return NULL;
}


static void reusableThreadMain( GBThread *self)
{
    assert(self);
    
    printf("[Worker] Start \n");
    
    
    usleep(5000);
    //sleep(1);
    
    printf("[Worker] End \n");
}

void testThread2()
{
    printf("----- Test Thread2 ----\n");
    
    GBThread* reusableThread = GBThreadInit();
    
    assert(GBThreadSetMain(reusableThread, reusableThreadMain));
    
    int count = 50;
    
    while (count)
    { 
        printf("----- Iteration %i -----\n" , count);
        assert(GBThreadStart(reusableThread));
        GBThreadWaitForStart(reusableThread);
        GBThreadWaitForStart(reusableThread);
        
        while (GBThreadIsRunning(reusableThread))
        {
            //printf("[Main] wait for thread terminaison ...\n");
            //sleep(1);
        }
        assert(GBThreadIsRunning(reusableThread) == 0);
        count--;
    }
    
    GBRelease(reusableThread);
    
    printf("[Main] returned \n");
}



