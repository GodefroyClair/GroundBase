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
 * \file GBJSON.hpp
 * \brief Json parse/write C++ API. See GBJSON.h for C API
 */

#ifndef GBJSON_hpp
#define GBJSON_hpp

#include <GBJSON.h>
#include <GBVariant.hpp>
namespace GB
{
    namespace JSON
    {
        
        /*!
         * @discussion Tries to parse a JSON formatted string and create an appropriate GB::Variant in return.
         * @param buffer A JSON formatted string buffer. Will return an invalid Variant if buffer is NULL.
         * @param size the string size of the buffer parameter. Will return NULL if size is 0 or GBSizeInvalid.
         * @return a GB::Variant instance on success. On error the variant returned will be invalid.
         */
        inline GB::Variant Parse( const char* buffer , std::size_t size)
        {
            return GB::Variant( GBJSONParse( buffer , size) , false );
        }
        
        /*!
         * @discussion Tries to generate a JSON formatted string from a GB::Variant instance.
         * @param root The object to serialize. This is safe to pass a NULL pointer (NULL will be returned).
         * @return a head allocated string, or NULL on error. You must call `free` on the returned pointer to avoid memory leak.
         */

        inline char* writeBuffer( const GB::Variant& root)
        {
            return GBJSONWriteBuffer( root.getCObject());
        }

    }
}

#endif /* GBJSON_hpp */
