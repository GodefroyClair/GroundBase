/*
 * Copyright (c) 2016-2018 FlyLab
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
//  GBRunLoopSource.h
//  GroundBase
//
//  Created by Manuel Deneu on 19/10/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

/**
 * \file GBRunLoopSource.h
 * \brief GBRunLoopSource is a basing type that represents input sources of a GBRunLoop.
 */

#ifndef GBRunLoopSource_h
#define GBRunLoopSource_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include <GBTypes.h>

/* **** **** **** */

/*!
 * @discussion An opaque type representing an abstract source for the GBRunLoop class. Concrete types are GBFDSource, GBTimer, etc.
 */
typedef GBObject GBRunLoopSource;

/*!
 * @discussion A notification id passed in a runloop source callback. This id represents the reason why the callback is invoked.
 */
typedef enum
{
    GBRunLoopSourceNotificationUnknown    = 0,  /*! Unknwon, should not happen & will be removed from API at some point*/
    
    GBRunLoopSourceCanRead                = 10, /*! Bytes to read are available.                                                  */
    
    GBRunLoopSourceDisconnected           = 11, /* Source has been disconnected -> you should remove it from the runloop  */
    GBRunLoopSourceError                  = 12, /* i/o error*/
    GBRunLoopSourceTimerFired             = 20, /*! Time update*/
    
} GBRunLoopSourceNotification;


/*!
 * @discussion The generic form for the run loop source callback.
 * @param source the source that fire the callback.
 * @param notification the reason of the notification.
 */
typedef void (*GBRunLoopSourceCallback)( GBRunLoopSource* source , GBRunLoopSourceNotification notification);

/*!
 * @discussion Returns the attached runloop.
 * @param source the source to get the runloop from. Will return NULL if NULL.
 * @return the attached runloop. Can be NULL.
 */
GBObject* GBRunLoopSourceGetRunLoop(const GBRunLoopSource* source );

/*
 Attach a pointer to the object. The context pointer is not used by the object, can be NULL or invalid.
 Has no effect if source is NULL
 */
void  GBRunLoopSourceSetUserContext(GBRunLoopSource* source , void* context );

/*
 Return the user pointer set by `GBRunLoopSourceSetUserContext`
 Has no effect if source is NULL
 */
void* GBRunLoopSourceGetUserContext( const GBRunLoopSource* source);

#ifdef __cplusplus
}
#endif
    
#endif /* GBRunLoopSource_h */
