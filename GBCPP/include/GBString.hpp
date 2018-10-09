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
 * \file GBString.hpp
 * \brief GBString  C++ API. See GBString.hpp for C API.
 * Note : The GroundBase C++ use std::string as primary string object. The GB::String object serves mostly for bridging between C and C++ API.
 */

#ifndef GBString_hpp
#define GBString_hpp

#include <string>
#include <GBString.h>
#include <GBObject.hpp>


// usefull to pass GBString as arguments from std::string
#define GBStrMake( stdString) GB::String(stdString).getAs<const GBString>()

namespace GB
{
    class String : public Object<GBString>
    {
    public:
        
        String( const std::string &str = "") GB_NO_EXCEPT:
        Object(GBStringInitWithCStr(str.c_str()))
        {}
        
        String& operator=(const std::string &str) GB_NO_EXCEPT
        {
            release();
            _ptr = GBStringInitWithCStr(str.c_str());
            return *this;
        }
        
        std::string toStdString() const GB_NO_EXCEPT
        {
            return GBStringGetCStr( _ptr);
        }
        
        bool isEmpty() const GB_NO_EXCEPT
        {
            return GBStringIsEmpty( _ptr );
        }
        
        std::size_t getLength() const GB_NO_EXCEPT
        {
            return GBStringGetLength(_ptr);
        }
        
        bool operator==(const String& rhs) GB_NO_EXCEPT
        {
            return GBStringEquals(_ptr, rhs._ptr);
        }
        
        bool operator==(const std::string &rhs) GB_NO_EXCEPT
        {
            return GBStringEqualsCStr(_ptr, rhs.c_str() );
        }
        
    };
}

#endif /* GBString_hpp */
