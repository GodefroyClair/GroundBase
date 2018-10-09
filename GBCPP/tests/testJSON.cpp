//
//  testJSON.cpp
//  UnitTestsCPP
//
//  Created by Manuel Deneu on 22/11/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include <string.h>
#include <stdio.h>
#include <GBJSON.hpp>
#include "TestBase.hpp"

bool testJSON()
{
    
    /* Invalid parses*/
    {
        const GB::Variant v1 = GB::JSON::Parse(nullptr, 0);
        assert(v1.isNull());
    }
    {
        const GB::Variant v1 = GB::JSON::Parse(nullptr, 10);
        assert(v1.isNull());
    }
    {
        const GB::Variant v1 = GB::JSON::Parse("^", 1);
        assert(v1.isNull());
    }
    
    /* Invalid get buffer*/
    {
        const GB::Variant nullV;
        assert( GB::JSON::writeBuffer(nullV) == nullptr);
    }
    
    {
        const GB::Variant dict
        ({
            {"key1" ,GB::Variant( "Value1" )},
            {"key2" ,GB::Variant( "Value2" )},
            {"key3" ,GB::Variant( 1 )},
            {"key4" ,GB::Variant( 2.f )},
            {"key5" ,GB::Variant( -3 )},
        });
        
        char* buf = GB::JSON::writeBuffer(dict);
        assert(buf);
        printf("Buffer '%s'\n" , buf);
        
        const GB::Variant dictParsed = GB::JSON::Parse(buf, strlen(buf));
        free(buf);
        
        assert(dictParsed.isValid());
        assert(dictParsed.isConvertibleToDictionary());
        
        
        assert( dictParsed == dict);
        
    }
    
    return true;
}
