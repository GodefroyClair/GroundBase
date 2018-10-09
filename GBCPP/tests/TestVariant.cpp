//
//  TestVariant.cpp
//  UnitTestsCPP
//
//  Created by Manuel Deneu on 03/11/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include <stdio.h>
#include <GBObject.hpp>
#include <GBVariant.hpp>
#include "TestBase.hpp"


static bool testInvalidVariant( const GB::Variant &vNull)
{
    
    assert(vNull.isValid() == false);
    assert(vNull.isNull() );
    
    assert(vNull.isConvertibleToString() == false);
    
    return true;
}

static bool testVariantList()
{
    GB::Variant list({
                        GB::Variant("str1"),
                        GB::Variant("str2"),
                        GB::Variant("str3"),
                     });
    
    
    assert(list.isValid());
    assert(list.isConvertibleToList() );
    assert(list.isConvertibleToNumber() == false );
    assert(list.isConvertibleToString() == false );
    assert(list.isConvertibleToDictionary() == false );
    
    for( const GB::Variant& v : list.getList())
    {
        assert(v.isValid());
    }

    return true;
}

static bool testVariantDict()
{
    GB::Variant dict({
                        {"key1",GB::Variant(1)},
                        {"key2",GB::Variant(1)}
                      });
    
    assert(dict.isConvertibleToDictionary());
    assert(dict.isConvertibleToList()   == false );
    assert(dict.isConvertibleToNumber() == false );
    assert(dict.isConvertibleToString() == false );
    
    for (const auto &p : dict.getDictionary())
    {
        assert(p.second.isValid());
        
        assert( (p.first == "key1" || p.first == "key2" ) && p.second.isConvertibleToNumber() && p.second.getInt() == 1 );
    }
    
    return true;
}

static bool testVariantBridge()
{
    
    GBDictionary* cDict = GBDictionaryInit();
    
    const GBSize numKeys = 10;
    
    for( GBIndex i =0; i<numKeys ; i++)
    {
        const GBString* k = GBStringInitWithFormat("Key%zi" , i);
        const GBNumber* num = GBNumberInitWithInt((int) i);
        assert(k && num);
        assert(GBDictionaryAddValueForKey(cDict, num, k) );
        
        GBRelease(k);
        GBRelease(num);
    }
    
    assert(GBDictionaryGetSize(cDict) == numKeys);
    
    {
        GB::Variant dict(cDict);
        assert(dict.isConvertibleToDictionary());
        assert(dict.getSize() == numKeys);
        
        const GB::Variant::Dictionary d = dict.getDictionary();
        
        assert(d.size() == numKeys);
        
        for( GBIndex i =0; i<numKeys ; i++)
        {
            const std::string key = "Key" + std::to_string(i);
            
            assert(d.count(key) == 1);
            assert(d.at(key).getInt() == (int)i);
        }
        
    }
    
    assert(GBDictionaryGetSize(cDict) == numKeys);
    
    
    GBRelease(cDict);
    return true;
}
bool testVariant()
{
    GB::Variant vNull;
    assert(vNull.isValid() == false);
    assert(vNull.isNull() );
    
    testInvalidVariant(vNull);
    
    assert(vNull.isConvertibleToString() == false);
    assert(vNull.isConvertibleToNumber() == false);
    
    GB::Variant vStr("TestHello");
    assert(vStr.isValid() );
    assert(vStr.isNull() == false);
    
    assert(vStr.isConvertibleToString());
    assert(vStr.isConvertibleToNumber() == false);
    
    GB::Variant vStr2 = vStr;
    
    assert(vStr == vStr2);
    assert(vStr2.isValid() );
    assert(vStr2.isNull() == false);
    
    assert(vStr2.isConvertibleToString() );
    assert(vStr2.isConvertibleToNumber() == false);
    
    vStr2 = GB::Variant((float) 132.123f);
    
    assert(vStr != vStr2);
    // vStr must not have been changed
    assert(vStr.isValid() );
    assert(vStr.isNull() == false);
    
    assert(vStr.isConvertibleToString());
    assert(vStr.isConvertibleToNumber() == false);
    
    assert(vStr2.getFloat() == 132.123f);
    assert(vStr2.isConvertibleToString() == false );
    assert(vStr2.isConvertibleToNumber() );
    
    
    vStr2 = nullptr;
    assert(vStr2.isNull() );
    
    testVariantList();
    testVariantDict();
    testVariantBridge();
    
    {
        GB::Variant v1(1230);
        GB::Variant v2(1230);
        
        GB::Variant v3("test");
        assert( v1 == v2);
        assert( v1 != v3);
        assert( v2 != v3);
    }
    {
        GB::Variant v1(-1230.f);
        GB::Variant v2(-1230.f);
        
        GB::Variant v3("test");
        assert( v1 == v2);
        
        assert( v1 != v3);
        assert( v2 != v3);
    }
    {
        GB::Variant v1("test");
        GB::Variant v2("test");
        
        GB::Variant v3(-1230.f);
        assert( v1 == v2);
        
        assert( v1 != v3);
        assert( v2 != v3);
    }
    {
        GB::Variant v1(1230);
        GB::Variant v2(1230.f);
        
        assert( v1 == v2);
    }
    {
        GB::Variant v( static_cast<long>(1233));
        GB::Variant v2( static_cast<int>(1233));
        
        assert(v.toLong() == static_cast<long>(1233));
        assert(v2.toLong() == v.toLong() );
    }
    
    return true;
}
