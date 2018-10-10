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
 * \file GBObject.hpp
 * \brief Json parse/write C++ API. See GBJSON.h for C API
 */

#ifndef GBObject_hpp
#define GBObject_hpp

#include <GBObject.h>
#include <cstddef> // std::size_t

#define GB_NO_EXCEPT noexcept

namespace GB
{
    // wil assert if GBRelease is called on an invalid object.
    static inline void enableInvalidReleaseDebug( bool enable)
    {
        GBRuntimeEnableInvalidReleaseDebug( enable);
    }
    
    // will return the number of GBObjects allocated at the time of the invocation.
    // will log on std::out a short decripion if showLog is true.
    static inline std::size_t Introspection( bool showLog)
    {
        return GBObjectIntrospection( showLog);
    }
    
    template< typename C_Obj >
    class Object
    {
    public:

        /*
         Usefull to create scoped objects
         
         eg:
         void foo()
         {
            GB::Object<GBArray> myArray(GBDictionaryGetKeyList(_ptr));
         
            ... Use `myArray` here ...
         
         } // myArray will automaticaly be released here.
         */
        explicit Object( C_Obj* obj) GB_NO_EXCEPT :
        _ptr(obj)
        {
        }
        
        Object( const Object &other) GB_NO_EXCEPT:
        _ptr(static_cast<C_Obj*>( GBObjectClone(other._ptr) ))
        {
        }
        
        
        Object& operator=(const Object &other) GB_NO_EXCEPT
        {
            if( _ptr)
                release();
            
            _ptr = static_cast<C_Obj*>( GBObjectClone(other._ptr) );
            return *this;
        }
        
        virtual ~Object()
        {
            if( _ptr)
            {
                GBRelease(_ptr);
            }
        }
        
        bool operator==(const Object& rhs) GB_NO_EXCEPT
        {
            return GBObjectEquals(_ptr, rhs._ptr);
        }
        
        bool operator!=(const Object& rhs) GB_NO_EXCEPT
        {
            return GBObjectEquals(_ptr, rhs._ptr) == 0;
        }

        /*!
         * @discussion Tests if a GB::Object ref is an instance of a certain class.
         * @param cClass the class to test (e.g GBStringClass, GBNumberClass)
         * @return true if obj inherits from _class.
         */
        bool isKindOf( GBObjectClassRef cClass) const GB_NO_EXCEPT
        {
            return IsKindOfClass(_ptr, cClass);
        }
        
        GBRef getGBRef() const GB_NO_EXCEPT
        {
            return _ptr;
        }
        
        /*
            Returns its underlying C Object.
         */
        template< typename C_ObjT> C_ObjT* getAs() const
        {
            return _ptr;
        }
        
        GBObjectClassRef getClass() const GB_NO_EXCEPT
        {
            return GBObjectGetClass(_ptr);
        }
        
        /*
        bool lock()
        {
            return GBObjectLock(_ptr);
        }
        
        bool tryLock()
        {
            return GBObjectTryLock(_ptr);
        }
        
        bool unlock()
        {
            return GBObjectUnlock(_ptr);
        }
         */

    protected:

        void retain() GB_NO_EXCEPT
        {
            GBRetain(_ptr);
        }
        
        void release() GB_NO_EXCEPT
        {
            GBRelease(_ptr);
        }
        
        C_Obj* _ptr;
    };
    
    
}


#endif /* GBObject_hpp */
