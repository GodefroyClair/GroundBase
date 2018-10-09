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
 * \file GBSharedMemory.hpp
 * \brief SharedMemory read/write C++ API. See GBSharedMem.h for C API
 */

#ifndef GBSharedMemory_hpp
#define GBSharedMemory_hpp

#include <GBString.hpp>
#include <GBObject.hpp>
#include <GBSharedMem.h>

namespace GB
{
    namespace SharedMemory
    {
        /*
         Base class, do no use directly, see GB::SharedMem::Reader & GB::SharedMem::Writer below
         */
        class Base : public Object<GBSharedMem>
        {
        public:
            
            Base& operator=(const Base &other) GB_NO_EXCEPT
            {
                if( _ptr)
                    release();
                
                _ptr = other._ptr;
                retain();
                
                return *this;
            }
            virtual ~Base(){}
            
            bool isValid() const GB_NO_EXCEPT
            {
                return getGBRef() != nullptr && GBSharedMemIsValid( getAs< GBSharedMem >());
            }
            
            std::size_t getSize() const GB_NO_EXCEPT
            {
                return GBSharedMemGetSize( getAs< GBSharedMem >());
            }
            
            
            // be sure to call `isValid()` before.
            // Will fail if the object is invalid.
            std::string getIdentifier() const GB_NO_EXCEPT
            {
                return GBStringGetCStr( GBSharedMemGetIdentifier( getAs< GBSharedMem >()) );
            }
        protected:
            // ctor for Writer
            Base( const std::string &identifier , std::size_t memorySize):
            Object( GBSharedMemInitWriter(GBStrMake(identifier) , memorySize ))
            {}
            
            Base( const std::string &identifier ):
            Object( GBSharedMemInitReader(GBStrMake(identifier)  ))
            {}
        };
        
        /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
        
        class Reader : public SharedMemory::Base
        {
        public:
            Reader( const std::string &identifier):
            Base(identifier)
            {}
            
            const void* getReadHandle() const GB_NO_EXCEPT
            {
                return GBSharedMemGetReadHandle( getAs< GBSharedMem >() );
            }

        private:
        };
        
        
        class Writer : public SharedMemory::Base
        {
        public:
            Writer( const std::string &identifier , std::size_t memSize):
            Base(identifier , memSize)
            {}
            
            void* getWriteHandle()
            {
                return GBSharedMemGetWriteHandle( getAs< GBSharedMem >()  );
            }
            
            
        private:
        };
    }
}


#endif /* GBSharedMemory_hpp */
