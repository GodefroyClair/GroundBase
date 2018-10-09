/*
 * Copyright (c) 2017-2018 FlyLab
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

/**
 * \file GBTimer.hpp
 * \brief GB::Timer : C++ object. See GBTimer.h for C API.
 */

#ifndef GBTimer_hpp
#define GBTimer_hpp

#include <functional>

#include <GBTimer.h>
#include <GBObject.hpp>
#include <GBRunLoop.hpp>

namespace GB
{
    /*!
     * @discussion a GBTimer instance to use in conjonction with a GB::RunLoop. You should not use Timers, it prevents CPUs from going Idle, and drain the battery.
     At least use Large timer intervals, or spawn a worked thread.
     */
    class Timer : public GB::RunLoopSource
    {
    public:
        
        /*!
         * @discussion The  form for a timer callback.
         */
        using TimeOutCallback = std::function<void(void)>;
        
        /*!
         * @discussion Creates a new timer. This timer is by default periodic and has a timeout of zero, so is invalid.
         * @param callback the nofication callback to use. Can't be NULL ! See GB::TimeOutCallback
         * @param intervalMS the interval is ms. must be > 0. Note that this interval is merly a hint, the real interval might vary.
         */
        Timer( TimeOutCallback callback , GBTimeMS intervalMS);
        
        /*!
         * @discussion Checks if the timer is active, or not. Default behaviour is true (active).
         * @return true if the timer is active, false if not.
         */
        bool isActive() const GB_NO_EXCEPT
        {
            return GBTimerIsActive( static_cast<const GBTimer*>( _ptr) );
        }
        
        /*!
         * @discussion Sets the timer to be active, or not. Default behaviour is true (active).
         * @param active true to set active (default behaviour), false to desactivate.
         */
        void setActive( bool active) GB_NO_EXCEPT
        {
            GBTimerSetActive( static_cast<GBTimer*>( _ptr), active?1:0);
        }
        
        /*!
         * @discussion Sets the interval -or the timeout in case of a single shot timer- between two timer ticks.
         * @param intervalMS the interval is ms. must be > 0. Note that this interval is merly a hint, the real interval might vary.
         */
        void setInterval( GBTimeMS intervalMS) GB_NO_EXCEPT
        {
            GBTimerSetIntervalMS( static_cast< GBTimer*>(_ptr), intervalMS);
        }
        
        /*!
         * @discussion Returns the interval -or the timeout in case of a single shot timer- between two timer ticks.
         * @return the set interval.
         */
        GBTimeMS getInterval() const GB_NO_EXCEPT
        {
            return GBTimerGetIntervalMS(static_cast< const GBTimer*>(_ptr));
        }
                
    private:
        TimeOutCallback _callback;
    };
}


inline GB::Timer::Timer( TimeOutCallback callback , GBTimeMS intervalMS):
GB::RunLoopSource(GBTimerInit([](  GBRunLoopSource* source , GBRunLoopSourceNotification notification)
                              {
                                  GB::Timer* self = reinterpret_cast<GB::Timer*>(GBRunLoopSourceGetUserContext(source));
                                  
                                  
                                  if( notification == GBRunLoopSourceTimerFired)
                                  {
                                      (self->_callback)();
                                  }
                                  
                              })),
_callback(callback)
{
    GBRunLoopSourceSetUserContext(_ptr, this);
    setInterval(intervalMS);
}

#endif /* GBTimer_hpp */
