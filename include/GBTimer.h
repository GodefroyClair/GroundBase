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
//  GBTimer.h
//  GroundBase
//
//  Created by Manuel Deneu on 07/10/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//


/**
 * \file GBTimer.h
 * \brief GBTimer : C object. See GBTimer.hpp for C++ API.
 */

#ifndef GBTimer_h
#define GBTimer_h

#ifdef __cplusplus
extern "C" {
#endif

//#include <sys/time.h>
#include <GBObject.h>
#include <GBRunLoopSource.h>

extern GBObjectClassRef GBTimerClass;
#define GBTimerClassName (const char*) "GBTimer"

/*!
 * @discussion An opaque GBTimer instance to use in conjonction with a GBRunLoop. You should not use Timers, it prevents CPUs from going Idle, and drain the battery.
 At least use Large timer intervals, or spawn a worked thread.
 */
typedef struct _GBTimer GBTimer;

/*!
 * @discussion The  form for a timer callback. This is just an alias for GBRunLoopSourceCallback.
 */
typedef GBRunLoopSourceCallback GBTimerCallback;

/*!
 * @discussion Creates a new timer. This timer is by default periodic and has a timeout of zero, so is invalid.
 * @param callback the nofication callback to use. Can't be NULL ! See GBTimerCallback.
 * @return A fully initialized instance of GBTimer.
 */
GBTimer* GBTimerInit(GBTimerCallback callback);

/*!
 * @discussion Sets the interval -or the timeout in case of a single shot timer- between two timer ticks.
 * @param timer the timer. Will return 0 if NULL
 * @param ms the interval is ms. must be > 0. Note that this interval is merly a hint, the real interval might vary.
 */
void GBTimerSetIntervalMS( GBTimer* timer , GBTimeMS ms);
    
/*!
 * @discussion Returns the interval -or the timeout in case of a single shot timer- between two timer ticks.
 * @param timer the timer. Will crash if NULL.
 * @return the set interval.
 */
GBTimeMS GBTimerGetIntervalMS( const GBTimer* timer) GB_NO_NULL_POINTERS;

/*!
 * @discussion Returns whether the timer is periodic. A timer is periodic by default.
 * @param timer the timer. Will crash if NULL.
 * @return 1 if the timer is periodic, 0 if single shot or error.
 */
BOOLEAN_RETURN uint8_t GBTimerIsPeriodic(const GBTimer* timer);
    
/*!
 * @discussion Sets the timer to be periodic, or not, ie single shot.
 * @param timer the timer. Nothing will happen if NULL.
 * @param periodic 1 to set periodic (default behaviour), 0 to set to single shot.
 */
void GBTimerSetPeriodic(GBTimer* timer , uint8_t periodic);

/*!
 * @discussion Checks if the timer is active, or not. Default behaviour is 1 (active).
 * @param timer the timer. Will crash if NULL.
 * @return 1 if the timer is active, 0 if not.
 */
BOOLEAN_RETURN uint8_t GBTimerIsActive(const GBTimer* timer) GB_NO_NULL_POINTERS;
    
/*!
 * @discussion Sets the timer to be active, or not. Default behaviour is 1 (active).
 * @param timer the timer. Nothing will happen if NULL.
 * @param active 1 to set active (default behaviour), 0 to desactivate.
 */
void GBTimerSetActive( GBTimer* timer , uint8_t active);

#ifdef __cplusplus
}
#endif

#endif /* GBTimer_h */
