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
 * \file GBVariant.hpp
 * \brief C++ Class cluster implementation for various data types. Has no counterpart in the C API.
 */

#ifndef GBVariant_hpp
#define GBVariant_hpp

#include <list>
#include <unordered_map>
#include <string>
#include <GBString.hpp>
#include <GBNumber.h>
#include <GBDictionary.h>
#include <GBArray.h>

namespace GB
{
    /*!
     * @discussion A variant is a wrapper around many types, including strings , fixed/float numbers, dictionaries and lists.
     */
    class Variant
    {
    public:
        
        /*!
         * @discussion Defines a list of GB::Variant values.
         */
        using List = std::list<Variant>;
        
        /*!
         * @discussion Defines a dictionary (i.e a associative key/value container) of GB::Variant values.
         */
        using Dictionary =  std::unordered_map<std::string, Variant>;
        
        /*!
         * @discussion Creates a variant from a C GBObject. Note that this constructor is mostly for bridging C++/C APIs. You should not use this one!
         * @param object a C GBObject. Can be NULL. If so the variant will be NULL/Invalid. See `isNull()`/`isValid()`
         * @param retain if true, the object will be retained (if not NULL).
         */
        explicit Variant( GBObject* object = nullptr , bool retain = true):
        _impl(object)
        {
            if( retain)
            {
                retainIfNeeded();
            }
        }
        
        ~Variant()
        {
            releaseIfNeeded();
        }

        /*!
         * @discussion Constructs a new Variant from an int.
         * @param val the value to store.
         */
        explicit Variant( int val):
        _impl( GBNumberInitWithInt(val) ) {}

        /*!
         * @discussion Constructs a new Variant from a float.
         * @param val the value to store.
         */
        explicit Variant( float val):
        _impl( GBNumberInitWithFloat(val) ) {}

        /*!
         * @discussion Constructs a new Variant from a double.
         * @param val the value to store.
         */
        explicit Variant( double val):
        _impl( GBNumberInitWithDouble(val) ) {}

        /*!
         * @discussion Constructs a new Variant from a long.
         * @param val the value to store.
         */
        explicit Variant( long val):
        _impl( GBNumberInitWithLong(val) ) {}

        /*!
         * @discussion Constructs a new Variant from a string.
         * @param val the value to store.
         */
        explicit Variant( const std::string &val):
        _impl(GBStringInitWithCStr(val.c_str())) {}
        
        /*!
         * @discussion Constructs a new Variant from a Variant list.
         * @param list the list to store.
         */
        explicit Variant( const GB::Variant::List &list):
        _impl(GBArrayInit())
        {
            for(auto v : list)
            {
                GBArrayAddValue(reinterpret_cast<GBArray*>( _impl), v._impl);
            }
        }

        /*!
         * @discussion Constructs a new Variant from a Variant Dictionary.
         * @param dict the dict to store.
         */
        explicit Variant( const GB::Variant::Dictionary &dict):
        _impl( GBDictionaryInit() )
        {
            for(auto p : dict)
            {
                GBDictionaryAddValueForKey( reinterpret_cast<GBDictionary*>(_impl), p.second._impl, GBStrMake(p.first) );
            }
        }
        
        /* **** **** **** **** **** **** **** **** **** **** */

        /*!
         * @discussion Copy constructor.Constructs a new Variant from another one. Not that the underlying value will be shared.
         * @param other a variant to copy.
         */
        Variant( const GB::Variant &other):
        _impl(other._impl)
        {
            retainIfNeeded();
        }
        
        /*!
         * @discussion Assignement operator. Assigns a new Variant from another one. Not that the underlying value will be shared, and the previous value will be released, if any.
         * @param rhs a variant to copy.
         */
        GB::Variant& operator=( const GB::Variant &rhs)
        {
            releaseIfNeeded();
            _impl = rhs._impl;
            retainIfNeeded();
            
            return *this;
        }
        
        /*!
         * @discussion Assignement operator. Assigns a new Variant from a C GBObject. Not that the underlying value will be shared, and the previous value will be released, if any.
         * @param cObject a variant to copy.
         */
        GB::Variant& operator=( GBObject* cObject)
        {
            releaseIfNeeded();
            _impl = cObject;
            retainIfNeeded();
            
            return *this;
        }
        
        bool operator==( const GB::Variant &rhs) const
        {
            return GBObjectEquals( _impl, rhs._impl );
        }
        
        bool operator != (const GB::Variant &rhs) const
        {
            return GBObjectEquals( _impl, rhs._impl ) == false;
        }
        
        /*!
         * @discussion Tests if a variant is valid, i.e if it has an underlying value.
         * @return true/false.
         */
        bool isValid() const GB_NO_EXCEPT
        {
            return _impl;
        }
        
        /*!
         * @discussion Tests if a variant is null, i.e if it has no underlying value.
         * @return true/false.
         */
        bool isNull() const GB_NO_EXCEPT
        {
            return _impl == nullptr;
        }
        
        /*!
         * @discussion Checks if the value can be converted to a string
         * @return true/false.
         */
        bool isConvertibleToString() const GB_NO_EXCEPT
        {
            if( _impl)
            {
                if( IsKindOfClass(_impl, GBStringClass)) return true;
            }
            return false;
        }
        
        /*!
         * @discussion Checks if the value can be converted to a number
         * @return true/false.
         */
        bool isConvertibleToNumber() const GB_NO_EXCEPT
        {
            if( _impl)
            {
                if( IsKindOfClass(_impl, GBNumberClass)) return true;
            }
            return false;
        }
        
        /*!
         * @discussion Checks if the value can be converted to a list
         * @return true/false.
         */
        bool isConvertibleToList() const GB_NO_EXCEPT
        {
            if( _impl)
            {
                if( IsKindOfClass(_impl, GBArrayClass)) return true;
            }
            return false;
        }
        
        /*!
         * @discussion Checks if the value can be converted to a dictionary.
         * @return true/false.
         */
        bool isConvertibleToDictionary() const GB_NO_EXCEPT
        {
            if( _impl)
            {
                if( IsKindOfClass(_impl, GBDictionaryClass)) return true;
            }
            return false;
        }
        
        
        /*!
         * @discussion Returns the size of variant (dimension might be a better name!) Singleton values (ie String/Numbers/NULL will return 0, Arrays/Dictionaries will return the number of elements;
         * @return true/false.
         */
        std::size_t getSize() const GB_NO_EXCEPT
        {
            if( _impl)
            {
                if( IsKindOfClass(_impl, GBDictionaryClass))
                    return GBDictionaryGetSize( reinterpret_cast<const GBDictionary*>(_impl) );
                else if( IsKindOfClass(_impl, GBArrayClass))
                    return GBArrayGetSize( reinterpret_cast<const GBArray*>( _impl));
            }
            return 0;
        }
        
        std::string getString() const
        {
            if( IsKindOfClass(_impl, GBStringClass))
                return GBStringGetCStr( reinterpret_cast<const GBString*>( _impl) );
            
            return "";
        }
        
        float getFloat() const
        {
            if( IsKindOfClass(_impl, GBNumberClass))
                return GBNumberGetFloat( reinterpret_cast<const GBNumber*>( _impl) );
            
            return 0.f;
        }
        
        double getDouble() const
        {
            if( IsKindOfClass(_impl, GBNumberClass))
                return GBNumberGetDouble( reinterpret_cast<const GBNumber*>( _impl) );
            
            return 0.;
        }
        
        long getLong() const
        {
            if( IsKindOfClass(_impl, GBNumberClass))
                return GBNumberGetLong( reinterpret_cast<const GBNumber*>( _impl) );
            
            return 0;
        }
        
        int getInt() const
        {
            if( IsKindOfClass(_impl, GBNumberClass))
                return GBNumberGetInt( reinterpret_cast<const GBNumber*>( _impl) );
            
            return 0;
        }
        
        const List getList() const GB_NO_EXCEPT
        {
            List ret;
            if( IsKindOfClass(_impl, GBArrayClass))
            {
                GBRef v = nullptr;
                GBArrayForEach( reinterpret_cast<const GBArray*>(_impl), v)
                {
                    ret.push_back(GB::Variant( const_cast<GBObject*>( v ) ));
                }
            }
            return ret;
        }
        
        const Dictionary getDictionary() const GB_NO_EXCEPT
        {
            Dictionary dict;
            if( IsKindOfClass(_impl, GBDictionaryClass))
            {
                GBDictionaryIterateValues(reinterpret_cast<const GBDictionary*>(_impl), [](const GBSequence* , const GBString* key , GBRef value , void* context)
                                          {
                                              Dictionary* dict = reinterpret_cast<Dictionary*>(context);
                                              dict->insert( { GBStringGetCStr(key) , GB::Variant( const_cast<GBObject*>( value ) )});
                                              
                                              return 1;
                                          }, &dict);
            }
            
            return dict;
        }
        
        
        bool toBool() const GB_NO_EXCEPT
        {
            return static_cast<bool>(toInt());
        }
        int toInt() const GB_NO_EXCEPT
        {
            if( isConvertibleToNumber())
                return GBNumberToInt( reinterpret_cast<const GBNumber*>( _impl ));
            
            return 0;
        }
        
        float toFloat() const GB_NO_EXCEPT
        {
            if( isConvertibleToNumber())
                return GBNumberToFloat( reinterpret_cast<const GBNumber*>( _impl ));
            
            return 0.f;
        }
        
        double toDouble() const GB_NO_EXCEPT
        {
            if( isConvertibleToNumber())
                return GBNumberToDouble( reinterpret_cast<const GBNumber*>( _impl ));
            
            return 0.;
        }
        
        long toLong() const GB_NO_EXCEPT
        {
            if( isConvertibleToNumber())
                return GBNumberToLong( reinterpret_cast<const GBNumber*>( _impl ));
            
            return 0;
        }
        
        // For those who really, _REALLY_ know what they are doing !
        // Be sure to retain C-object if you want to keep it arround.
        GBObject* getCObject() const
        {
            return _impl;
        }

    private:
        
        void releaseIfNeeded()
        {
            if( _impl)
            {
                GBRelease(_impl);
            }
        }
        
        void retainIfNeeded() const
        {
            if( _impl)
            {
                GBRetain(_impl);
            }
        }
        
        GBObject* _impl;
    };
}

#endif /* GBVariant_hpp */
