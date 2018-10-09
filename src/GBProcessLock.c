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
//  GBProcessLock.c
//  GroundBase
//
//  Created by Manuel Deneu on 30/07/2016.
//

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h> // open
#include <unistd.h> // lockf F_TLOCK

#include <GBProcessLock.h>
#include <GBCommons.h>


#define PIDFILE_PATH ( const char*) "/var/tmp/"

uint8_t GBProcessLock( int *pidFileHandler , const GBString* name )
{
    if( pidFileHandler == NULL )
    {
        DEBUG_LOG("INVALID PID Handler \n");
        return 0;
    }
#warning Should use MAX_FILE_PATH !
    char pidFile[80];
    strcpy(pidFile, PIDFILE_PATH );
    strcat(pidFile,  GBStringGetCStr( name ) );
    strcat(pidFile, ".pid");
    
    *pidFileHandler = open( pidFile, O_RDWR|O_CREAT, 0600);
    
    if( *pidFileHandler != -1)
    {
        if (lockf( *pidFileHandler,F_TLOCK,0) != -1)
        {
            char str[10];
            
            sprintf(str,"%d\n",getpid());
            
            write( *pidFileHandler, str, strlen(str));
            return 1;
        }
    }
    
    return 0;
}

uint8_t GBProcessUnLock( int *pidFileHandler )
{
    if( pidFileHandler == NULL)
        return 0;
    
    const int ret = close(*pidFileHandler);
    
    *pidFileHandler = -1;
    
    return ret == 0;
}
