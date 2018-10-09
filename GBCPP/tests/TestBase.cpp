//
//  TestBase.cpp
//  GroundBaseCPP
//
//  Created by Manuel Deneu on 03/11/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include "TestBase.hpp"


#include <unistd.h>
#include <iostream>
#include <GroundBase.hpp>
#include <GBObject.hpp>
#include <GBString.hpp>
#include <GBDictionary.hpp>
#include <GBArray.hpp>
#include <GBRunLoop.hpp>
#include <GBTimer.hpp>


bool testDictionary()
{
    
    GB::Dictionary dict;
    
    assert(dict.isKindOf(GBDictionaryClass));
    
    assert(dict.getSize() == 0);
    assert(dict.isEmpty() );
    
    assert(dict.contains("lolz") == false);
    assert(dict.contains("") == false);
    
    
    assert(dict.removeKey("test") == false);
    
    assert(dict.addValue(12,  "int1"));
    assert(dict.addValue(12,  "int1") == false);
    
    assert(dict.removeKey("test") == false);
    
    assert(dict.getSize() == 1);
    assert(dict.isEmpty() == false);
    
    assert(dict.addValue(123.f,  "float2"));
    assert(dict.addValue(123.f,  "float2") == false);
    
    assert(dict.removeKey("test") == false);
    
    assert(dict.getSize() == 2);
    assert(dict.isEmpty() == false);
    
    assert(dict.addValue(123.1,  "double3"));
    assert(dict.addValue(123.1,  "double3") == false);
    
    long val = 6789;
    
    assert(dict.addValue(val,  "long4"));
    assert(dict.addValue(val,  "long4") == false);
    
    
    assert(dict.contains("int1"));
    assert(dict.contains("float2"));
    assert(dict.contains("double3"));
    assert(dict.contains("long4"));
    
    assert(dict.contains("lolz") == false);
    
    
    GB::Dictionary::KeyList list = dict.getKeys();
    
    size_t count = 0;
    for(auto key : list)
    {
        printf("Got Key '%s' \n" , key.c_str());
        
        const GBObject* obj = dict.getValueForKey<GBObject>(key);
        assert(GBObjectIsValid(obj));
        
        if( key == "int1")
        {
            assert(IsKindOfClass(obj, GBNumberClass));
            auto v = GBNumberGetInt(static_cast<const GBNumber*>(obj));
            assert(v == 12);
            count++;
        }
        else if( key == "float2")
        {
            assert(IsKindOfClass(obj, GBNumberClass));
            auto v = GBNumberGetFloat(static_cast<const GBNumber*>(obj));
            assert(v == 123.f);
            count++;
        }
        else if( key == "double3")
        {
            assert(IsKindOfClass(obj, GBNumberClass));
            auto v = GBNumberGetDouble(static_cast<const GBNumber*>(obj));
            assert(v == 123.1);
            count++;
        }
        else if( key == "long4")
        {
            assert(IsKindOfClass(obj, GBNumberClass));
            auto v = GBNumberGetLong(static_cast<const GBNumber*>(obj));
            assert(v == 6789);
            count++;
        }
    }
    
    assert(count == dict.getSize());
    assert(list.size() == dict.getSize());
    
    {
        // the 2 object are now different
        GB::Dictionary dict2 = dict;
        
        assert(dict.removeKey("long4"));
        
        
        assert(dict.getSize() == dict2.getSize() -1 );
        
        const GBNumber* v1 = dict2.getValueForKey<GBNumber>("int1");
        assert(v1);
        assert(IsKindOfClass(v1, GBNumberClass));
        
        assert(dict2.removeKey("long4"));
        
        assert(dict2.getSize() == dict.getSize());
        
        assert(dict2.clear());
        assert(dict2.isEmpty());
        assert(dict2.getSize() == 0);
    }
    // dict2 have been cleared, dict must remain valid
    
    assert(dict.getSize() == 3);
    
    
    return true;
}


bool testString()
{
    GB::String str("hello");
    GB::String str2;
    
    assert(str.toStdString() == "hello");
    assert(str2.toStdString() =="");
    
    {
        str2 = str;
        
        assert(str.isKindOf(GBStringClass));
        assert(str2.isKindOf(GBStringClass));
        
        assert( str.toStdString() == "hello");
        assert( str == str2);
        assert( str == str2.toStdString());
    }
    
    assert( str.toStdString() == "hello");
    assert(str.isKindOf(GBStringClass));
    
    str = "Changed the underlying string";
    
    assert( str.toStdString() == "Changed the underlying string");
    
    assert(str != str2);
    
    return true;
    
}

bool testArray()
{
    GB::Array array;
    assert(array.getSize() == 0);
    assert(array.isEmpty());
    
    assert(array.removeValue("lolz") == false);
    
    /*
     const GBString* str = GBStringInitWithCStr("Hello");
     array.addValue(str);
     assert(GBObjectGetRefCount(str) == 2);
     */
    assert(array.addValue("Hello"));
    assert(array.isEmpty() ==false);
    assert(array.addValue(1));
    assert(array.isEmpty() ==false);
    assert(array.addValue(2.f));
    assert(array.isEmpty() ==false);
    assert(array.addValue(3.));
    assert(array.isEmpty() ==false);
    assert(array.addValue((long ) 67890));
    assert(array.isEmpty() ==false);
    
    assert(array.getSize() == 5);
    
    for(size_t i= 0;i<array.getSize() ;i++)
    {
        auto obj = array.getValueAtIndex<GBObject>(i);
        
        assert(GBObjectIsValid(obj) );
        
        if( i==0)
        {
            assert(IsKindOfClass(obj, GBStringClass));
        }
        else
        {
            assert(IsKindOfClass(obj, GBNumberClass));
        }
    }
    
    assert(array.removeValue(  GBStrMake( "Hello" ) ));
    assert(array.getSize() == 4);
    
    
    assert(array.clear());
    assert(array.getSize() == 0);
    return true;
}


bool testLambdaString()
{
    GB::String s ("HelloTest");
    
    auto fct1 = []( const GB::String &str)
    {
        assert(str.toStdString() == "HelloTest");
    };
    
    auto fct2 = []( const GB::String str)
    {
        assert(str.toStdString() == "HelloTest");
    };
    
    auto fct3 = [s]( )
    {
        assert(s.toStdString() == "HelloTest");
    };
    
    auto fct4 = [&s]( )
    {
        assert(s.toStdString() == "HelloTest");
    };
    
    fct1(s);
    fct2(s);
    
    fct3();
    fct4();
    
    
    return true;
}


