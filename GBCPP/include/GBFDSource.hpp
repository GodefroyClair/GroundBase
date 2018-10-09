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
 * \file GBFDSource.hpp
 * \brief  GB::FDSource : C++ object that wrap a file descriptor source. See GBFDSource.h for C API.
 */

#ifndef GBFDSource_hpp
#define GBFDSource_hpp

#include <functional>


#include <GBObject.hpp>
#include <GBFDSource.h>
#include <GBRunLoop.hpp>
namespace GB
{
    class FDSource : public GB::RunLoopSource
    {
    public:
        
        
        /*!
         * @discussion Creates a new GBFDSource instance. GBFDSource is provided as a simple way to add externals (from GroundBase point of view) file descriptors to a GB::RunLoop instance.
         * @param fd an opened & ready to use user file descriptor.
         * @param callback the nofication callback to use. Must be valid is attached later to a runloop. If used for send operations only can be NULL.
         Warning : By default you must close the file descriptor yourself. See `shouldCloseOnDestruct` method to let GB::FDSource's destructor close the descriptor for you
         */
        FDSource( int fd , SourceCallback callback = nullptr):
        GB::RunLoopSource(GBFDSourceInitWithFD(fd, [] (GBRunLoopSource* source , GBRunLoopSourceNotification notification)
                                    {
                                        GB::FDSource* self = reinterpret_cast<GB::FDSource*>(GBRunLoopSourceGetUserContext(source));
                                        
                                        
                                        self->sourceCallback( (Notification) notification );
                                    })),
        sourceCallback(callback)
        {
            GBRunLoopSourceSetUserContext(_ptr, this);
        }
        
        /*!
         * @discussion Returns the underlying file descriptor number.
         * @return The file descriptor number on sucess, -1 on error.
         */
        int getFileDescriptor() const  GB_NO_EXCEPT
        {
            return GBFDSourceGetFileDescriptor( static_cast<GBFDSource*>(_ptr) );
        }
        
        /*!
         * @discussion Attempts to read size bytes of data from the source. See man 2 read for more general informations about file descriptor reads.
         * @param content a valid address to copy the read buffer to. Will return GBSizeInvalid if NULL, undefined behaviour if invalid.
         * @param sizeToRead the number of bytes to read. The effective read operation might read less bytes.
         * @return If successful, the number of bytes actually read, GBSizeInvalid otherwise.
         */
        std::size_t read( void* content,  std::size_t sizeToRead) GB_NO_EXCEPT
        {
            return GBFDSourceRead( static_cast<GBFDSource*>(_ptr), content ,sizeToRead);
        }
        
        std::size_t send( const void* data , std::size_t dataLength ,int flags)
        {
            return GBFDSourceSend(  static_cast<GBFDSource*>(_ptr), data, dataLength , flags);
        }
        
        std::size_t write( const void* data , std::size_t dataLength)
        {
            return GBFDSourceWrite( static_cast<GBFDSource*>(_ptr) , data , dataLength);
        }

        void shouldCloseOnDestruct( bool shouldClose)
        {
            GBFDSourceShouldCloseOnDestruct( static_cast<GBFDSource*>(_ptr)  , shouldClose);
        }
        
        
        SourceCallback sourceCallback;
        
    };
    
}

#endif /* GBFDSource_hpp */
