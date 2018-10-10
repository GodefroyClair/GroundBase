/*
 * Copyright (c) 2017 FlyLab
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
 * \file GBRunLoop.hpp
 * \brief GB::RunLoop : C++ object that handles sources such as timers or sockets an provides a event driven model. See GBRunLoop.h for C API.
 */

#ifndef GBRunLoop_hpp
#define GBRunLoop_hpp

#include <functional>
#include <GBRunLoop.h>

#include <GBObject.hpp>
#include <GBRunLoopSource.h>

namespace GB
{
    class RunLoop; // forward def
    
    class RunLoopSource : public Object<GBRunLoopSource>
    {
    public:
        
        /*!
         * @discussion A notification id passed in a runloop source callback. This id represents the reason why the callback is invoked. This is an alias for GBRunLoopSourceNotification.
         */
        typedef enum : int
        {
            CanRead = GBRunLoopSourceCanRead,
            Disconnected = GBRunLoopSourceDisconnected,
            Error = GBRunLoopSourceError
            
        } Notification;
        
        /*!
         * @discussion The generic form for the run loop source callback.
         * @param notification the reason of the notification.
         */
        using SourceCallback = std::function<void(Notification notification)>;
        
        virtual ~RunLoopSource()
        {}
        
        /*!
         * @discussion Returns the attached runloop.
         * @return the attached runloop. Can be invalid.
         */
        GB::RunLoop getRunLoop();
        
    protected:
        RunLoopSource(GBRunLoopSource* source):
        Object(source)
        {}
    };
    
    /*!
     * @discussion GB::RunLoop is the C++ counterpart of GBRunLoop
     */
    class RunLoop : public Object<GBRunLoop>
    {
    public:
        
        
        /*!
         * @discussion Creates a GBRunLoop instance from a C GBRunLoop instance.
         * @param rl a GBRunLoop instance. Undefined behaviour if NULL or invalid. This instance will be retained.
         */
        RunLoop( GBRunLoop*rl):
        Object(rl )
        {
            retain();
        }
        
        /*!
         * @discussion Creates a new GB::RunLoop instance.
         */
        RunLoop():
        Object(GBRunLoopInit() )
        {}
        
        /*!
         * @discussion Runs the runloop until `GB::RunLoop::stop` is called.
         * @return false if rl is NULL, or block until `GBRunLoopStop` is called and will return true.
         */
        bool run()
        {
            return GBRunLoopRun(_ptr);
        }
        
        /*!
         * @discussion Stops the runloop. This is safe to call this method any other thread.
         * @return false if rl is NULL or the runloop is not running, true on sucess. Note that the stop might be async, and thus not immediate on return.
         */
        bool stop()
        {
            return GBRunLoopStop(_ptr);
        }
        
        GB_DEPRECATED("5.3.7") bool isRunning() const
        {
            return GBRunLoopIsRunning(_ptr);
        }
        
        /*!
         * @discussion Returns the number of added timers, active or not.
         * @return the number of added timers.
         */
        std::size_t getNumTimers() const
        {
            return GBRunLoopGetNumTimers(_ptr);
        }
        
        /*!
         * @discussion Returns the number of added file descriptor source.
         * @return the number of added file descriptor.
         */
        std::size_t getNumFDSources() const
        {
            return GBRunLoopGetNumFDSources(_ptr);
        }
        
        /*!
         * @discussion Returns the total number of sources, ie `GB::RunLoop::getNumTimers + GB::RunLoop::getNumFDSources`
         * @return the total number of sources.
         */
        std::size_t getNumSources() const
        {
            return GBRunLoopGetNumSources(_ptr);
        }
        
        /*!
         * @discussion Adds a source for the runloop. Note : the source must have been removed from any previous runloop or indefined behaviour will occur.
         * @param source a C runloop source.
         * @return false if rl or source is NULL, true on sucess.
         */
        bool addSource( GBRunLoopSource* source)
        {
            return GBRunLoopAddSource(_ptr, source);
        }

        /*!
         * @discussion Adds a source for the runloop. Note : the source must have been removed from any previous runloop or indefined behaviour will occur.
         * @param source a C++ runloop source.
         * @return false if rl or source is NULL, true on sucess.
         */
        bool addSource( GB::RunLoopSource &source)
        {
            return addSource(source.getAs<GBRunLoopSource>());
        }
        
        /*!
         * @discussion Removes a source for the runloop.
         * @param source a C runloop source.
         * @return false if rl, or source is NULL, or the source does not belong the this runloop, true on sucess.
         */
        bool removeSource( GBRunLoopSource* source)
        {
            return GBRunLoopRemoveSource(_ptr, source);
        }
        
        /*!
         * @discussion Checks if a source belongs to a runloop.
         * @param source a runloop source.
         * @return false if rl, or source is NULL, or the source does not belong the this runloop, true if the source belongs to the runloop rl.
         */
        bool containsSource( const GB::RunLoopSource &source) const GB_NO_EXCEPT
        {
            return GBRunLoopContainsSource(_ptr , source.getAs<GBRunLoopSource>() );
        }
        
        /* Async/After calls */
        
        /*!
         * @discussion The general callback form to pass to GB::RunLoop::async/after
         */
        using AsyncCallback = std::function<void()>;
        
        /*!
         * @discussion Executes a callback asynchronously on a runloop.
         * @param method a method to call. Must have a signature following 'GB::RunLoop::AsyncCallback' convention.
         * @return false if rl is NULL, true if the call suceeds.
         */
        bool async( AsyncCallback method)
        {
            // FIXME Will leak if the runloop is stopped before the callback is invoked -> delete will never be called
            AsyncCallback *m = new AsyncCallback(method); // must make a copy since the callback can go out of scope.
            
            return GBRunLoopDispatchAsync(_ptr , [](GBRunLoop*  , void* data )
                   {
                       AsyncCallback *m = reinterpret_cast<AsyncCallback *>(data);
                       (*m)();
                       
                       delete m;
                   }, m );
        }
        
        /*!
         * @discussion Executes a callback asynchronously on a runloop, after a certain delay.
         * @param method a method to call. Must have a signature following 'GB::RunLoop::AsyncCallback' convention.
         * @param timeOutMs the time to wait before actualy invoking the callback. Not that his time is approximate, and might take a little longer than expected.
         * @return false if rl is NULL, true if the call suceeds.
         */
        bool after( AsyncCallback method , GBTimeMS timeOutMs)
        {
            // FIXME Will leak if the runloop is stopped before the callback is invoked -> delete will never be called
            AsyncCallback *m = new AsyncCallback(method); // must make a copy since the callback can go out of scope.
            
            return GBRunLoopDispatchAfter(_ptr , [](GBRunLoop*  , void* data )
                                          {
                                              AsyncCallback *m = reinterpret_cast<AsyncCallback *>(data);
                                              (*m)();
                                              
                                              delete m;
                                          }, m ,timeOutMs);
        }
        
        

        
    };
}

inline GB::RunLoop GB::RunLoopSource::getRunLoop()
{
    return GB::RunLoop( static_cast<GBRunLoop*>( GBRunLoopSourceGetRunLoop(_ptr) ) );
}


#endif /* GBRunLoop_hpp */
