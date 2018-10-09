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
 * \file GBPropertyList.hpp
 * \brief See GBPropertyList.h for C API
 * A property list file is basically a XML document, with type checks (ie a 'type' attribute)
 * You can't add any random values to it, only Basic types ( string, numbers, containers)
 */

#ifndef GBPropertyList_hpp
#define GBPropertyList_hpp

#include <GBPropertyList.h>
#include <GBObject.hpp>
#include <GBString.hpp>
#include <GBDictionary.hpp>
#include <string>


namespace GB
{
    class PropertyList : public Object<GBPropertyList>
    {
    public:
        
        /*!
         * @discussion Initialize an empty GB::PropertyList instance.
         * @return an empty GB::PropertyList instance. Use `GB::PropertyList::writeRootObject` to add content.
         */
        PropertyList():
        Object(GBPropertyListInit() )
        {}
        
        
        /*!
         * @discussion Initialize an  GB::PropertyList instance from the content of a file.
         * @param filePath An accessible filePath to a valid PropertyList file. Will be invalid if empty.
         * @return an initialized GB::PropertyList instance. Use `GB::PropertyListGetRoot` to access content.
         */
        PropertyList( const std::string &filePath):
        Object( GBPropertyListInitWithFile( GBStrMake( filePath) ) )
        {}
        
        /*!
         * @discussion Checks if the property list is valid.
         * @return true/false
         */
        bool isValid() const GB_NO_EXCEPT
        {
            return _ptr;
        }
        
        /*!
         * @discussion Add a root section to the propertyList. This section will be called `plist`.
         * @param dict a valid GB::Dictionary instance, that will be cloned. will return false if invalid.
         * @return true on sucess, false on error.
         */
        bool writeRootObject( const GB::Dictionary &dict) GB_NO_EXCEPT
        {
            return GBPropertyListWriteRootObject( _ptr , dict.getAs< GBDictionary > () );
        }
        
        /*!
         * @discussion Saves the property list to a file, given its filepath.
         * @param filePath An accessible filePath to a writable file. Will return false if invalid.
         * @return true on sucess, false on error. Will return false if the property list is empty or invalid.
         */
        bool saveFile( const std::string &filePath) GB_NO_EXCEPT
        {
            return GBPropertyListSaveFile( _ptr, GBStrMake( filePath) );
        }
        
        /*!
         * @discussion returns the content of a propertyList.
         * @return the root content as a GB::Dictionary (might be empty).
         * Will return an empty dict. if the property list is empty or invalid.
         */
        GB::Dictionary getRoot() const GB_NO_EXCEPT
        {
            return GBPropertyListGetRoot(_ptr );
        }
        
    };
}

#endif /* GBPropertyList_hpp */
