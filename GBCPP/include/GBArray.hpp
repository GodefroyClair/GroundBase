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
 * \file GBArray.hpp
 * \brief C++ GBObject's Array . See GBArray.h for C API
 */

#ifndef GBArray_hpp
#define GBArray_hpp

#include <GBArray.h>
#include <GBObject.hpp>

namespace GB
{
    class Array : public Object<GBArray>
    {
    public:
        Array():
        Object( GBArrayInit())
        {}
        
        std::size_t getSize() const GB_NO_EXCEPT
        {
            return GBArrayGetSize(_ptr);
        }
        
        bool isEmpty() const GB_NO_EXCEPT
        {
            return GBArrayGetSize(_ptr) == 0;
        }
        
        bool addValue( GBRef value  ) GB_NO_EXCEPT
        {
            return GBArrayAddValue(_ptr, value );
        }
        
        bool addValue( int value ) GB_NO_EXCEPT
        {
            return GBArrayAddValue(_ptr, GB::Object<GBNumber>(GBNumberInitWithInt(value)).getGBRef());
        }
        
        bool addValue( float value ) GB_NO_EXCEPT
        {
            return GBArrayAddValue(_ptr, GB::Object<GBNumber>(GBNumberInitWithFloat(value)).getGBRef());
        }
        
        bool addValue( double value ) GB_NO_EXCEPT
        {
            return GBArrayAddValue(_ptr, GB::Object<GBNumber>(GBNumberInitWithDouble(value)).getGBRef() );
        }
        
        bool addValue( long value ) GB_NO_EXCEPT
        {
            return GBArrayAddValue(_ptr, GB::Object<GBNumber>(GBNumberInitWithLong(value)).getGBRef() );
        }
        
        bool addValue( const std::string &value ) GB_NO_EXCEPT
        {
            return GBArrayAddValue(_ptr, GBStrMake(value));
        }
        
        bool addValue( const char* value ) GB_NO_EXCEPT
        {
            return GBArrayAddValue(_ptr, GBStrMake(value));
        }
        
        bool removeValue( GBRef value)
        {
            return GBArrayRemoveValue(_ptr, value);
        }
        
        template< typename C_OBJ>
        const C_OBJ* getValueAtIndex( std::size_t index) const GB_NO_EXCEPT
        {
            return static_cast<const C_OBJ*>( GBArrayGetValueAtIndex(_ptr, index) );
        }
        
        bool removeValue( const std::string &value ) GB_NO_EXCEPT
        {
            return GBArrayRemoveValue(_ptr, GBStrMake(value));
        }

        bool clear() GB_NO_EXCEPT
        {
            return GBArrayClear(_ptr);
        }
        
    };
}

#endif /* GBArray_hpp */
