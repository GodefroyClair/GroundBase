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
 * \file GBDictionary.hpp
 * \brief C++ Key-based dictionary. See GBDictionary.h for C API
 */

#ifndef GBDictionary_hpp
#define GBDictionary_hpp

#include <unordered_map>
#include <list>

#include <GBDictionary.h>
#include <GBNumber.h>

#include <GBObject.hpp>
#include <GBString.hpp>

#include <GBVariant.hpp>

namespace GB
{
    class Dictionary : public Object<GBDictionary>
    {
    public:
        
        /*!
         * @discussion A list of keys.
         */
        using KeyList = std::list<std::string>;
        
        /*!
         * @discussion Creates an empty GB::Dictionary instance.
         */
        Dictionary():
        Object( GBDictionaryInit())
        {}
        
        // hackish
        Dictionary( const GBDictionary* cDict):
        Object( const_cast<GBDictionary*>( cDict ) )
        {
            retain();
        }
        
        /*!
         * @discussion Get the dictionary items count.
         * @return the dictionary item's count.
         */
        std::size_t getSize() const GB_NO_EXCEPT
        {
            return GBDictionaryGetSize(_ptr);
        }
        
        /*!
         * @discussion Check if a dictionary instance has items.
         * @return true if the dictionary is empty, false otherwise.
         */
        bool isEmpty() const GB_NO_EXCEPT
        {
            return GBDictionaryGetSize(_ptr) == 0;
        }
        
        /*!
         * @discussion Check if a value exists for a given key
         * @param key the key to search.
         * @return true if the value exists, false otherwise.
         */
        bool contains(const std::string &key) const GB_NO_EXCEPT
        {
            return GBDictionaryContains(_ptr, GBStrMake(key) );
        }
        
        /*!
         * @discussion Add a GB::Variant value for a given key
         * @param value the value to add.
         * @param key the associed key.
         * @return true if the key/value association is added
         */
        bool addValue( const GB::Variant &value , const std::string &key) GB_NO_EXCEPT
        {
            return GBDictionaryAddValueForKey(_ptr, value.getCObject(), GBStrMake(key));
        }
        
        /*!
         * @discussion Add a C GBObject value for a given key
         * @param value the value to add.
         * @param key the associed key.
         * @return true if the key/value association is added
         */
        bool addValue( GBRef value , const std::string& key ) GB_NO_EXCEPT
        {
            return GBDictionaryAddValueForKey(_ptr, value, GBStrMake(key) );
        }
        
        /*!
         * @discussion Add an int value for a given key
         * @param value the value to add.
         * @param key the associed key.
         * @return true if the key/value association is added
         */
        bool addValue( int value , const std::string& key ) GB_NO_EXCEPT
        {
            return GBDictionaryAddValueForKey(_ptr, GB::Object<GBNumber>(GBNumberInitWithInt(value)).getGBRef(), GBStrMake(key));
        }
        
        /*!
         * @discussion Add a float value for a given key
         * @param value the value to add.
         * @param key the associed key.
         * @return true if the key/value association is added
         */
        bool addValue( float value , const std::string& key ) GB_NO_EXCEPT
        {
            return GBDictionaryAddValueForKey(_ptr, GB::Object<GBNumber>(GBNumberInitWithFloat(value)).getGBRef(), GBStrMake(key));
        }
        
        /*!
         * @discussion Add a double value for a given key
         * @param value the value to add.
         * @param key the associed key.
         * @return true if the key/value association is added
         */
        bool addValue( double value , const std::string& key ) GB_NO_EXCEPT
        {
            return GBDictionaryAddValueForKey(_ptr, GB::Object<GBNumber>(GBNumberInitWithDouble(value)).getGBRef(), GBStrMake(key));
        }
        
        /*!
         * @discussion Add a long value for a given key
         * @param value the value to add.
         * @param key the associed key.
         * @return true if the key/value association is added
         */
        bool addValue( long value , const std::string& key ) GB_NO_EXCEPT
        {
            return GBDictionaryAddValueForKey(_ptr, GB::Object<GBNumber>(GBNumberInitWithLong(value)).getGBRef(), GBStrMake(key));
        }
        
        /*!
         * @discussion Add a std::string value for a given key
         * @param value the value to add.
         * @param key the associed key.
         * @return true if the key/value association is added
         */
        bool addValue( const std::string& value , const std::string& key ) GB_NO_EXCEPT
        {
            return GBDictionaryAddValueForKey(_ptr, GBStrMake(value), GBStrMake(key));
        }
        
        /*!
         * @discussion Remove a value for a given key
         * @param key the key to remove
         * @return true if the value is removed, false otherwise.
         */
        bool removeKey(const std::string &key) GB_NO_EXCEPT
        {
            return GBDictionaryRemove(_ptr, GBStrMake(key) );
        }
        
        /*!
         * @discussion Empties a dictionary. Each stored value will be released.
         * @return true if the operation succeded, false otherwise.
         */
        bool clear() GB_NO_EXCEPT
        {
            return GBDictionaryClear(_ptr);
        }
        
        /*!
         * @discussion Returns a list of keys contained in the dictionary.
         * @return a list of std::strings (empty if invalid).
         */
        KeyList getKeys() const GB_NO_EXCEPT
        {
            KeyList r;
            GB::Object<GBArray> l(GBDictionaryGetKeyList(_ptr));

            GBRef v = nullptr;
            GBArrayForEach(l.getAs<GBArray>(), v)
            {
                r.push_back( GBStringGetCStr( static_cast<const GBString*>(v)) );
            }
            
            return r;
        }
        
        /*!
         * @discussion Get a value associated to a key
         * @param key the key to get
         * @return a valid Ref to the value if found, NULL otherwise. Not, you need to explicitly retain the value if you want to keep a reference.
         */
        template< typename C_OBJ>
        const C_OBJ* getValueForKey( const std::string &key) const GB_NO_EXCEPT
        {
            return static_cast<const C_OBJ*>( GBDictionaryGetValueForKey(_ptr, GBStrMake(key)) );
        }

    };
}

#endif /* GBDictionary_hpp */
