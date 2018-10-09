//
//  testPropertyList.cpp
//  UnitTestsCPP
//
//  Created by Manuel Deneu on 28/11/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include <stdio.h>
#include "TestBase.hpp"
#include <GBPropertyList.hpp>

bool testPropertyList()
{
    {
        GB::PropertyList invalidPropList("nonExistingFile.lol");
        
        assert(invalidPropList.isValid() == false);
    }
    
    GB::PropertyList propList;
    {
        // valid prop list
        
        
        assert(propList.isValid() );
        
        
        GB::Dictionary dict;
        dict.addValue(GB::Variant("Test"), "keyTest");
        dict.addValue(GB::Variant("Test2"), "keyTest2");
        dict.addValue(GB::Variant(123.1f), "keyFloat");
        dict.addValue(GB::Variant(-123.123), "keyDouble");
        dict.addValue(GB::Variant(-12312), "keyInt");
        
        
        
        
        assert(propList.writeRootObject( dict));
        
        assert(propList.getRoot().contains("keyTest"));
        assert(propList.getRoot().contains("keyTest2"));
        assert(propList.getRoot().contains("keyFloat"));
        assert(propList.getRoot().contains("keyDouble"));
        assert(propList.getRoot().contains("keyInt"));
        
        assert(propList.saveFile("textCpp.plist"));
        
        
        
    }
    
    assert(propList.isValid()); // still valid
    
    {
        GB::PropertyList propListFromFile("textCpp.plist");
        assert(propListFromFile.isValid());
        
        const GB::Dictionary root = propListFromFile.getRoot();
        assert(propList.getRoot().getSize() == propListFromFile.getRoot().getSize());
        
        assert( root.contains("keyTest"));
        assert( root.contains("keyTest2"));
        assert( root.contains("keyFloat"));
        assert( root.contains("keyDouble"));
        assert( root.contains("keyInt"));
        
        assert(propList.getRoot() == root);
        
    }
    // test reading property list generated from C API
    {
        GB::PropertyList plist("text.plist");
        
        if( plist.isValid())
        {
            const GB::Dictionary root = plist.getRoot();
            
            std::size_t accum = 0;
            for( auto const &key : root.getKeys() )
            {
                assert(key.empty() == false);
                accum++;
            }
            
            assert(accum == root.getSize());
        }
                               
    }
    return true;
}

